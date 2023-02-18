/*
   MIT License

  Copyright (c) 2020-22 ElectroTechnique

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

------------------------------------------------
  ELECTROTECHNIQUE MOMENTUM - Firmware Rev 1.00
  Teensy MicroMod based Synthesizer - 12 voices
------------------------------------------------

  Arduino IDE Tools Settings:
    Board: "Teensy MicroMod"
    USB Type: "Serial + MIDI + Audio"
    CPU Speed: "600MHz"
    Optimize: "Faster"

  This code is based on TSynth V2.32 firmware - ElectroTechnique 2021
  and includes code by:
    Dave Benn - Handling MUXs, a few other bits and original inspiration  https://www.notesandvolts.com/2019/01/teensy-synth-part-10-hardware.html
    Alexander Davis / Vince R. Pearson - Stereo ensemble chorus effect https://github.com/quarterturn/teensy3-ensemble-chorus
    Will Winder - Major refactoring and monophonic mode
    Vince R. Pearson - Exponential envelopes & glide
    Github members fab672000 & CDW2000 - General improvements to code
    Github member luni64 - EncoderTool library modified and included https://github.com/luni64/EncoderTool
    Mark Tillotson - Special thanks for band-limiting the waveforms in the Audio Library

  Additional libraries:
    Janila Array, Adafruit_GFX (available in Arduino libraries manager)
*/

#include <Adafruit_GFX.h>
#include <ILI9341_t3n.h>
#include <vector>
#include "Audio.h" //Using local version to override Teensyduino version
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <MIDI.h>
#include <USBHost_t36.h>
#include <TeensyThreads.h>
#include <TeensyTimerTool.h>
using namespace TeensyTimerTool;
#include "MidiCC.h"
#include "SettingsService.h"
#include "AudioPatching.h"
#include "Constants.h"
#include "Parameters.h"

FLASHMEM void silence();
volatile boolean cardStatus = false;
volatile boolean sdCardInterrupt = false;
boolean usbHostPluggedIn = false;
elapsedMillis usb_host_wait_timer;

State state = State::MAIN;

#include "PatchMgr.h"
#include "HWControls.h"
#include "EepromMgr.h"
#include "Detune.h"
#include "utils.h"
#include "Voice.h"
#include "VoiceGroup.h"

// Initialize the audio configuration.
Global global{VOICEMIXERLEVEL};
// VoiceGroup voices1{global.SharedAudio[0]};
std::vector<VoiceGroup *> groupvec;
uint8_t activeGroupIndex = 0;

#include "StringMapping.h"
#include "PerformanceMgr.h"
#include "EncoderMapping.h"

// TCK : 5.00 seconds
// TCK64 : 634.20 years
// TCK_RTC : 634.20 years
// GPT(@24MHz) 178.96 seconds
// TMR(PSC_AUTO) 55.92 milliseconds
// PIT(@24MHz) 178.96 seconds
PeriodicTimer sequencer_timer(TCK); // TCK PIT GPT2
#include "SequenceMgr.h"
extern void sequencer();
extern void noteOnRoutine();

// USB HOST MIDI Class Compliant
USBHost myusb;
USBHub hub1(myusb);
USBHub hub2(myusb);
MIDIDevice midi1(myusb);
// MIDIDevice_BigBuffer midi1(myusb); // Try this if your MIDI Compliant controller has problems

// MIDI 5 Pin DIN - (TRS MIDI)
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

FLASHMEM void changeMIDIThruMode()
{
    MIDI.turnThruOn(MIDIThru);
}

FLASHMEM void myProgramChange(byte channel, byte program);
void myPitchBend(byte channel, int bend);
void myNoteOff(byte channel, byte note, byte velocity);
void myNoteOn(byte channel, byte note, byte velocity);
FLASHMEM void setCurrentPatchData();
FLASHMEM void recallPatch(uint8_t bank, long patchUID);
FLASHMEM void recallPerformance(uint8_t filename);
FLASHMEM void myControlChange(byte channel, byte control, byte value);
FLASHMEM void usbHostControlChange(byte channel, byte control, byte value);
FLASHMEM void myMIDIClock();
FLASHMEM void myMIDIClockContinue();
FLASHMEM void myMIDIClockStart();
FLASHMEM void myMIDIClockStop();

void midiCCOut(byte cc, byte value);
void encoderCallback(unsigned enc_idx, int value, int delta);
void encoderButtonCallback(unsigned button_idx, int state);
void buttonCallback(unsigned button_idx, int state);

#include "Display.h"
#include "Settings.h"

boolean firstPatchLoaded = false;

uint32_t previousMillis = millis(); // For MIDI Clk Sync

uint32_t midiClkCount = 0; // For MIDI Clk Sync
int voiceToReturn = -1;    // Initialise

uint8_t seq_last_step = 0;

FLASHMEM void setup()
{
    sequencer_timer.begin(sequencer, currentSequence.tempo_us / 2.0f, false);
    //  while (!Serial)
    //  {
    //  }
    //  Serial.print(CrashReport);
    AudioMemory(60);
    checkFirstRun();
    // Initialize the voice groups.
    uint8_t total = 0;
    while (total < global.maxVoices())
    {
        VoiceGroup *currentGroup = new VoiceGroup{global.SharedAudio[groupvec.size()]};

        for (uint8_t i = 0; total < global.maxVoices() && i < global.maxVoicesPerGroup(); i++)
        {
            Voice *v = new Voice(global.Oscillators[i], i);
            currentGroup->add(v);
            total++;
        }
        groupvec.push_back(currentGroup);
    }
    setUpSettings();
    cardStatus = SD.begin(BUILTIN_SDCARD);
    if (cardStatus)
    {
        Serial.println(F("SD card is connected"));
        // Get patch numbers and names from SD card
        checkSDCardStructure();
        loadBankNames();
        loadLastPatchUsed();
        loadPatchNamesFromBank(currentBankIndex);
        // If current patch is empty in this bank, go to previous bank with valid patch
        while (patches[0].patchUID == 0)
        {
            tempBankIndex = decTempBankIndex();
            loadPatchNamesFromBank(tempBankIndex);
            currentBankIndex = tempBankIndex;
            currentPatchIndex = 0;
        }
        recallPatch(currentBankIndex, patches[currentPatchIndex].patchUID);
    }
    else
    {
        setCurrentPatchData(); // Initialise to default
        Serial.println(F("SD card is not connected or unusable"));
    }

    // Read MIDI Channel from EEPROM
    midiChannel = getMIDIChannel();
    Serial.println(F("MIDI In Channel:") + String(midiChannel) + F(" (0 is Omni On)"));

    // USB HOST MIDI Class Compliant
    myusb.begin();
    midi1.setHandleControlChange(usbHostControlChange);
    midi1.setHandleNoteOff(myNoteOff);
    midi1.setHandleNoteOn(myNoteOn);
    midi1.setHandlePitchChange(myPitchBend);
    midi1.setHandleProgramChange(myProgramChange);
    midi1.setHandleClock(myMIDIClock);
    midi1.setHandleContinue(myMIDIClockContinue);
    midi1.setHandleStart(myMIDIClockStart);
    midi1.setHandleStop(myMIDIClockStop);
    Serial.println(F("USB HOST MIDI Class Compliant Listening"));

    // USB Client MIDI
    usbMIDI.setHandleControlChange(myControlChange);
    usbMIDI.setHandleNoteOff(myNoteOff);
    usbMIDI.setHandleNoteOn(myNoteOn);
    usbMIDI.setHandlePitchChange(myPitchBend);
    usbMIDI.setHandleProgramChange(myProgramChange);
    usbMIDI.setHandleClock(myMIDIClock);
    usbMIDI.setHandleContinue(myMIDIClockContinue);
    usbMIDI.setHandleStart(myMIDIClockStart);
    usbMIDI.setHandleStop(myMIDIClockStop);
    Serial.println(F("USB Client MIDI Listening"));

    // MIDI 5 Pin DIN
    MIDI.begin();
    MIDI.setHandleNoteOn(myNoteOn);
    MIDI.setHandleNoteOff(myNoteOff);
    MIDI.setHandlePitchBend(myPitchBend);
    MIDI.setHandleControlChange(myControlChange);
    MIDI.setHandleProgramChange(myProgramChange);
    MIDI.setHandleClock(myMIDIClock);
    MIDI.setHandleContinue(myMIDIClockContinue);
    MIDI.setHandleStart(myMIDIClockStart);
    MIDI.setHandleStop(myMIDIClockStop);
    Serial.println(F("MIDI In DIN Listening"));

    // Read MIDI Out Channel from EEPROM
    midiOutCh = getMIDIOutCh();
    // Read MIDI Thru mode from EEPROM
    MIDIThru = getMidiThru();
    changeMIDIThruMode();
    // Read oscilloscope enable from EEPROM
    enableScope(getScopeEnable());
    // Read VU enable from EEPROM
    vuMeter = getVUEnable();
    // Read volume  from EEPROM
    currentVolume = getVolume();
    // Read global tuning from EEPROM
    tuningCents = getTuningCents();
    tuningCentsFrac = 1.0f + (tuningCents * CENTSFRAC);

    assignStrings();
    assignParametersForPerformanceEncoders();
    setupDisplay();
    setupHardware(encoderCallback, encoderButtonCallback, buttonCallback);
    setEncodersState(state);
}

// Specifically for Sequencer, bypassing recording note in
void myNoteOnSeq(byte channel, byte note, byte velocity)
{
    groupvec[activeGroupIndex]->noteOn(note, velocity);
}

void myNoteOn(byte channel, byte note, byte velocity)
{
    if (currentSequence.recording)
    {
        currentSequence.note_in = note;
        currentSequence.note_in_velocity = velocity;
    }

    // Check for out of range notes
    if (note + groupvec[activeGroupIndex]->params().oscPitchA < 0 || note + groupvec[activeGroupIndex]->params().oscPitchA > 127 || note + groupvec[activeGroupIndex]->params().oscPitchB < 0 || note + groupvec[activeGroupIndex]->params().oscPitchB > 127)
        return;
    groupvec[activeGroupIndex]->noteOn(note, velocity);
}

void myNoteOff(byte channel, byte note, byte velocity)
{
    groupvec[activeGroupIndex]->noteOff(note);
}

FLASHMEM void updatePatchnameAndIndex(String name, uint32_t index, uint32_t UID)
{
    groupvec[activeGroupIndex]->setPatchName(name);
    groupvec[activeGroupIndex]->setPatchIndex(index);
    groupvec[activeGroupIndex]->setUID(UID);
    setPatchNoAndNameForDisp(String(index), name);
}

void myPitchBend(byte channel, int bend)
{
    groupvec[activeGroupIndex]->pitchBend(bend * currentPatch.PitchBend * DIV12xDIV8192);
}

// MIDI CC
void myControlChange(byte channel, byte control, byte value)
{
    // Serial.printf("Ch:%u: cc=%d, v=%d\n", channel, control, value);
    switch (control)
    {
    case CCvolume:
        currentVolume = value;
        storeVolumeToEEPROM(value);
        groupvec[activeGroupIndex]->setVolume(LINEAR[currentVolume] * MAX_VOL);
        showCurrentParameterOverlay(ParameterStrMap[CCvolume], LINEAR[currentVolume]);
        break;
    case CCunison:
    {
        currentPatch.Unison = value;
        groupvec[activeGroupIndex]->setUnisonMode(value);

        if (value == 0)
        {
            setEncValue(CCdetune, currentPatch.Detune, String((1 - groupvec[activeGroupIndex]->params().detune) * 100) + F("%"));
            if (!setEncValue(CCunison, value, F("Off")))
                showCurrentParameterOverlay(F("Unison"), F("Off"));
        }
        else if (value == 1)
        {
            setEncValue(CCdetune, currentPatch.Detune, String((1 - groupvec[activeGroupIndex]->params().detune) * 100) + "%");
            if (!setEncValue(CCunison, value, F("Dynamic")))
                showCurrentParameterOverlay(F("Dynamic"), F("On"));
        }
        else
        {
            setEncValue(CCdetune, currentPatch.ChordDetune, String(CDT_STR[groupvec[activeGroupIndex]->params().chordDetune]));
            if (!setEncValue(CCunison, value, F("Chord")))
                showCurrentParameterOverlay(F("Chord"), F("On"));
        }
        break;
    }
    case CCglide:
        currentPatch.Glide = value;
        groupvec[activeGroupIndex]->params().glideSpeed = POWER[value];
        if (!setEncValue(CCglide, value, milliToString(POWER[value] * GLIDEFACTOR)))
            showCurrentParameterOverlay(ParameterStrMap[CCglide], milliToString(POWER[value] * GLIDEFACTOR));
        break;

    case CCpitchenv:
    {
        currentPatch.PitchEnv = value;
        float env = LINEARCENTREZERO[value] * OSCMODMIXERMAX;
        groupvec[activeGroupIndex]->setPitchEnvelope(env);
        if (!setEncValue(CCpitchenv, value, String(env)))
            showCurrentParameterOverlay(ParameterStrMap[CCpitchenv], String(env));
        break;
    }
    case CCoscwaveformA:
        currentPatch.WaveformA = value;
        groupvec[activeGroupIndex]->setWaveformA(getWaveformA(value));
        if (!setEncValue(CCoscwaveformA, value, getWaveformStr(getWaveformA(value))))
            showCurrentParameterOverlay(ParameterStrMap[CCoscwaveformA], getWaveformStr(getWaveformA(value)));
        break;
    case CCoscwaveformB:
        currentPatch.WaveformB = value;
        groupvec[activeGroupIndex]->setWaveformB(getWaveformB(value));
        if (!setEncValue(CCoscwaveformB, value, getWaveformStr(getWaveformB(value))))
            showCurrentParameterOverlay(ParameterStrMap[CCoscwaveformB], getWaveformStr(getWaveformB(value)));
        break;

    case CCpitchA:
        currentPatch.PitchA = value;
        groupvec[activeGroupIndex]->params().oscPitchA = PITCH[value];
        groupvec[activeGroupIndex]->updateVoices();
        if (!setEncValue(CCpitchA, value, (PITCH[value] > 0 ? F("+") : F("")) + String(PITCH[value])))
            showCurrentParameterOverlay(ParameterStrMap[CCpitchA], (PITCH[value] > 0 ? F("+") : F("")) + String(PITCH[value]));
        break;

    case CCpitchB:
        currentPatch.PitchB = value;
        groupvec[activeGroupIndex]->params().oscPitchB = PITCH[value];
        groupvec[activeGroupIndex]->updateVoices();
        if (!setEncValue(CCpitchB, value, (PITCH[value] > 0 ? F("+") : F("")) + String(PITCH[value])))
            showCurrentParameterOverlay(ParameterStrMap[CCpitchB], (PITCH[value] > 0 ? F("+") : F("")) + String(PITCH[value]));
        break;

    case CCdetune:
    {
        currentPatch.Detune = value;
        float detune = 1.0f - (MAXDETUNE * POWER[value]);
        groupvec[activeGroupIndex]->params().detune = detune;
        groupvec[activeGroupIndex]->params().chordDetune = value;
        groupvec[activeGroupIndex]->updateVoices();

        if (groupvec[activeGroupIndex]->params().unisonMode == 2)
        {
            ;
            if (!setEncValue(CCdetune, value, CDT_STR[value]))
                showCurrentParameterOverlay(F("Chord"), CDT_STR[value]);
        }
        else
        {
            if (!setEncValue(CCdetune, value, String((1 - detune) * 100) + F("%")))
                showCurrentParameterOverlay(ParameterStrMap[CCdetune], String((1 - detune) * 100) + F("%"));
        }
        break;
    }
    case CCpwmSourceA:
        currentPatch.PWMSourceA = value;
        groupvec[activeGroupIndex]->setPWMSourceA(value);

        if (value == PWMSOURCELFO)
        {
            if (!setEncValue(CCpwmSourceA, value, F("LFO")))
                showCurrentParameterOverlay(ParameterStrMap[CCpwmSourceA], F("LFO"));
            // Turn on rate control
            setEncValue(true, CCpwmRateA, currentPatch.PWMRateA, String(PWMRATE[currentPatch.PWMRateA]) + F("Hz"), CCpwmRateA);
            setEncInactive(CCpwA);
        }
        else if (value == PWMSOURCEFENV)
        {
            if (!setEncValue(CCpwmSourceA, value, F("Filter Env")))
                showCurrentParameterOverlay(ParameterStrMap[CCpwmSourceA], F("Filter Env"));
            setEncInactive(CCpwmRateA);
            setEncInactive(CCpwA);
            // Turn on pwm amt control
            setEncValue(true, CCpwmAmtA, currentPatch.PWMA_Amount, String(LINEAR[currentPatch.PWMA_Amount]), CCpwmAmtA);
        }
        else
        {
            if (!setEncValue(CCpwmSourceA, value, F("Manual")))
                showCurrentParameterOverlay(ParameterStrMap[CCpwmSourceA], F("Manual"));
            setEncInactive(CCpwmRateA);
            setEncInactive(CCpwmAmtA);
            if (groupvec[activeGroupIndex]->getWaveformA() == WAVEFORM_TRIANGLE_VARIABLE)
            {
                setEncValue(true, CCpwA, currentPatch.PWA_Amount, F("Tri ") + String(groupvec[activeGroupIndex]->getPwA()), CCpwA);
            }
            else
            {
                setEncValue(true, CCpwA, currentPatch.PWA_Amount, F("Pulse ") + String(groupvec[activeGroupIndex]->getPwA()), CCpwA);
            }
        }
        break;
    case CCpwmSourceB:
        currentPatch.PWMSourceB = value;
        groupvec[activeGroupIndex]->setPWMSourceB(value);

        if (value == PWMSOURCELFO)
        {
            if (!setEncValue(CCpwmSourceB, value, F("LFO")))
                showCurrentParameterOverlay(ParameterStrMap[CCpwmSourceB], F("LFO"));
            // Turn on rate control
            setEncValue(true, CCpwmRateB, currentPatch.PWMRateB, String(PWMRATE[currentPatch.PWMRateB]) + F("Hz"), CCpwmRateB);
            setEncInactive(CCpwB);
        }
        else if (value == PWMSOURCEFENV)
        {
            if (!setEncValue(CCpwmSourceB, value, F("Filter Env")))
                showCurrentParameterOverlay(ParameterStrMap[CCpwmSourceB], F("Filter Env"));
            setEncInactive(CCpwB);
            setEncInactive(CCpwmRateB);
            // Turn on pwm amt controls
            setEncValue(true, CCpwmAmtB, currentPatch.PWMB_Amount, String(LINEAR[currentPatch.PWMB_Amount]), CCpwmAmtB);
        }
        else
        {
            if (!setEncValue(CCpwmSourceB, value, F("Manual")))
                showCurrentParameterOverlay(ParameterStrMap[CCpwmSourceB], F("Manual"));
            setEncInactive(CCpwmRateB);
            setEncInactive(CCpwmAmtB);
            if (groupvec[activeGroupIndex]->getWaveformB() == WAVEFORM_TRIANGLE_VARIABLE)
            {
                setEncValue(true, CCpwB, currentPatch.PWB_Amount, F("Tri ") + String(groupvec[activeGroupIndex]->getPwB()), CCpwB);
            }
            else
            {
                setEncValue(true, CCpwB, currentPatch.PWB_Amount, F("Pulse ") + String(groupvec[activeGroupIndex]->getPwB()), CCpwB);
            }
        }
        break;
    case CCpwmRateA:
        currentPatch.PWMRateA = value;
        groupvec[activeGroupIndex]->setPwmRateA(PWMRATE[value]);
        if (!setEncValue(CCpwmRateA, value, String(2 * PWMRATE[value]) + F(" Hz")))
            showCurrentParameterOverlay(ParameterStrMap[CCpwmRateA], String(2 * PWMRATE[value]) + F(" Hz")); // PWM goes through mid to maximum, sounding effectively twice as fast
        break;
    case CCpwmRateB:
        currentPatch.PWMRateB = value;
        groupvec[activeGroupIndex]->setPwmRateB(PWMRATE[value]);
        if (!setEncValue(CCpwmRateB, value, String(2 * PWMRATE[value]) + F(" Hz")))
            showCurrentParameterOverlay(ParameterStrMap[CCpwmRateB], String(2 * PWMRATE[value]) + F(" Hz")); // PWM goes through mid to maximum, sounding effectively twice as fast
        break;

    case CCpwmAmtA:
        currentPatch.PWMA_Amount = value;
        if (currentPatch.PWMSourceA == PWMSOURCELFO)
        {
            groupvec[activeGroupIndex]->setPwmMixerALFO(LINEAR[value]);
        }
        else if (currentPatch.PWMSourceA == PWMSOURCEFENV)
        {
            groupvec[activeGroupIndex]->setPwmMixerAFEnv(LINEAR[value]);
        }
        if (!setEncValue(CCpwmAmtA, value, String(LINEAR[value])))
            showCurrentParameterOverlay(ParameterStrMap[CCpwmAmtA], String(LINEAR[value]));
        break;

    case CCpwmAmtB:
        currentPatch.PWMB_Amount = value;
        if (currentPatch.PWMSourceB == PWMSOURCELFO)
        {
            groupvec[activeGroupIndex]->setPwmMixerBLFO(LINEAR[value]);
        }
        else if (currentPatch.PWMSourceB == PWMSOURCEFENV)
        {
            groupvec[activeGroupIndex]->setPwmMixerBFEnv(LINEAR[value]);
        }
        if (!setEncValue(CCpwmAmtB, value, String(LINEAR[value])))
            showCurrentParameterOverlay(ParameterStrMap[CCpwmAmtB], String(LINEAR[value]));
        break;

    case CCpwA:
        currentPatch.PWA_Amount = value;
        groupvec[activeGroupIndex]->setPWA(LINEARCENTREZERO[value]);
        if (groupvec[activeGroupIndex]->getWaveformA() == WAVEFORM_TRIANGLE_VARIABLE)
        {
            if (!setEncValue(CCpwA, value, F("Tri ") + String(groupvec[activeGroupIndex]->getPwA())))
                showCurrentParameterOverlay2("1 Var Triangle", groupvec[activeGroupIndex]->getPwA(), VAR_TRI);
        }
        else
        {
            if (!setEncValue(CCpwA, value, F("Pulse ") + String(groupvec[activeGroupIndex]->getPwA())))
                showCurrentParameterOverlay2("1 Pulse Width", groupvec[activeGroupIndex]->getPwA(), PULSE);
        }
        break;

    case CCpwB:
        currentPatch.PWB_Amount = value;
        groupvec[activeGroupIndex]->setPWB(LINEARCENTREZERO[value]);
        if (groupvec[activeGroupIndex]->getWaveformB() == WAVEFORM_TRIANGLE_VARIABLE)
        {
            if (!setEncValue(CCpwB, value, F("Tri ") + String(groupvec[activeGroupIndex]->getPwB())))
                showCurrentParameterOverlay2("2 Variable Triangle", groupvec[activeGroupIndex]->getPwB(), VAR_TRI);
        }
        else
        {
            if (!setEncValue(CCpwB, value, F("Pulse ") + String(groupvec[activeGroupIndex]->getPwB())))
                showCurrentParameterOverlay2("2 Pulse Width", groupvec[activeGroupIndex]->getPwB(), PULSE);
        }
        break;

    case CCoscLevelA:
        currentPatch.OscLevelA = value;
        groupvec[activeGroupIndex]->setOscLevelA(LINEAR[value]);
        switch (groupvec[activeGroupIndex]->getOscFX())
        {
        case 1: // XOR
            if (!setEncValue(CCoscLevelA, value, String(groupvec[activeGroupIndex]->getOscLevelA())))
                showCurrentParameterOverlay(F("Osc Mix 1:2"), "   " + String(groupvec[activeGroupIndex]->getOscLevelA()) + F(" : ") + String(groupvec[activeGroupIndex]->getOscLevelB()));
            break;
        case 2: // XMod
                // osc A sounds with increasing osc B mod
            if (!setEncValue(CCoscLevelA, value, String(groupvec[activeGroupIndex]->getOscLevelA())))
                showCurrentParameterOverlay(F("X-Mod Osc 1"), F("by Osc 2: ") + String(1 - groupvec[activeGroupIndex]->getOscLevelB()));
            break;
        case 0: // None
            if (!setEncValue(CCoscLevelA, value, String(groupvec[activeGroupIndex]->getOscLevelA())))
                showCurrentParameterOverlay(ParameterStrMap[CCoscLevelA], String(groupvec[activeGroupIndex]->getOscLevelA()));
            break;
        }
        break;

    case CCoscLevelB:
        currentPatch.OscLevelB = value;
        groupvec[activeGroupIndex]->setOscLevelB(LINEAR[value]);
        switch (groupvec[activeGroupIndex]->getOscFX())
        {
        case 1: // XOR
            if (!setEncValue(CCoscLevelB, value, String(groupvec[activeGroupIndex]->getOscLevelB())))
                showCurrentParameterOverlay(F("Osc Mix 1:2"), "   " + String(groupvec[activeGroupIndex]->getOscLevelA()) + F(" : ") + String(groupvec[activeGroupIndex]->getOscLevelB()));
            break;
        case 2: // XMod
                // osc B sounds with increasing osc A mod
            if (!setEncValue(CCoscLevelB, value, String(groupvec[activeGroupIndex]->getOscLevelB())))
                showCurrentParameterOverlay(F("X-Mod Osc 2"), F("by Osc 1: ") + String(1 - groupvec[activeGroupIndex]->getOscLevelA()));
            break;
        case 0: // None
            if (!setEncValue(CCoscLevelB, value, String(groupvec[activeGroupIndex]->getOscLevelB())))
                showCurrentParameterOverlay(ParameterStrMap[CCoscLevelB], String(groupvec[activeGroupIndex]->getOscLevelB()));
            break;
        }
        break;

    case CCnoiseLevel:
    {
        currentPatch.NoiseLevel = value;
        float noise = LINEARCENTREZERO[value];
        float pink = 0.0;
        float white = 0.0;
        if (noise > 0)
        {
            pink = noise;
        }
        else if (noise < 0)
        {
            white = fabs(noise);
        }
        groupvec[activeGroupIndex]->setPinkNoiseLevel(pink);
        groupvec[activeGroupIndex]->setWhiteNoiseLevel(white);

        if (noise > 0)
        {
            if (!setEncValue(CCnoiseLevel, value, F("Pink ") + String(pink)))
                showCurrentParameterOverlay(ParameterStrMap[CCnoiseLevel], F("Pink ") + String(pink));
        }
        else if (noise < 0)
        {
            if (!setEncValue(CCnoiseLevel, value, F("White ") + String(white)))
                showCurrentParameterOverlay(ParameterStrMap[CCnoiseLevel], F("White ") + String(white));
        }
        else
        {
            if (!setEncValue(CCnoiseLevel, value, F("Off")))
                showCurrentParameterOverlay(ParameterStrMap[CCnoiseLevel], F("Off"));
        }
        break;
    }
    case CCbankselectLSB:
        if (value < BANKS_LIMIT)
        {
            Serial.println(value);
            currentBankIndex = value;
            if (!cardStatus)
                return;
            loadPatchNamesFromBank(currentBankIndex);
            if (patches.size() > 0 && patches[0].patchUID > 0)
            {
                currentPatchIndex = 0;
                recallPatch(currentBankIndex, patches[0].patchUID);
            }
        }
        break;

    case CCfilterfreq:
        // MIDI is 7 bit, 128 values and needs to choose alternate filterfreqs(8 bit) by multiplying by 2
        currentPatch.FilterFreq = value << 1; // FilterFreq is 8 bit
        groupvec[activeGroupIndex]->setCutoff(FILTERFREQS256[currentPatch.FilterFreq]);
        if (!setEncValue(filterfreq256, currentPatch.FilterFreq, String(int(FILTERFREQS256[currentPatch.FilterFreq])) + F(" Hz")))
            showCurrentParameterOverlay(ParameterStrMap[CCfilterfreq], String(int(FILTERFREQS256[currentPatch.FilterFreq])) + F(" Hz"));
        break;

    case filterfreq256:
        // 8 bit from panel control for smoothness
        currentPatch.FilterFreq = value;
        groupvec[activeGroupIndex]->setCutoff(FILTERFREQS256[value]);
        if (!setEncValue(filterfreq256, value, String(int(FILTERFREQS256[value])) + F(" Hz")))
            showCurrentParameterOverlay(ParameterStrMap[CCfilterfreq], String(int(FILTERFREQS256[value])) + F(" Hz"));
        break;

    case CCfilterres:
    {
        currentPatch.FilterRes = value;
        float res = (14.29f * POWER[value]) + 0.71f;
        groupvec[activeGroupIndex]->setResonance(res);
        if (!setEncValue(CCfilterres, value, res))
            showCurrentParameterOverlay(ParameterStrMap[CCfilterres], res);
        break;
    }
    case CCfiltermixer:
    {
        currentPatch.FilterMixer = value;
        groupvec[activeGroupIndex]->setFilterMixer(LINEAR_FILTERMIXER[value]);

        String filterStr;
        if (LINEAR_FILTERMIXER[value] == BANDPASS)
        {
            filterStr = F("Band Pass");
        }
        else
        {
            // LP-HP mix mode - a notch filter
            if (LINEAR_FILTERMIXER[value] == LOWPASS)
            {
                filterStr = F("Low Pass");
            }
            else if (LINEAR_FILTERMIXER[value] == HIGHPASS)
            {
                filterStr = F("High Pass");
            }
            else
            {
                filterStr = F("Low ") + String(100 - int(100 * LINEAR_FILTERMIXER[value])) + F(" - ") + String(int(100 * LINEAR_FILTERMIXER[value])) + F(" High");
            }
        }
        if (!setEncValue(CCfiltermixer, value, filterStr))
            showCurrentParameterOverlay(ParameterStrMap[CCfiltermixer], filterStr);

        break;
    }

    case CCfilterenv:
    {
        currentPatch.FilterEnv = value;
        float env = LINEARCENTREZERO[value] * FILTERMODMIXERMAX;
        groupvec[activeGroupIndex]->setFilterEnvelope(env);
        if (!setEncValue(CCfilterenv, value, String(env)))
            showCurrentParameterOverlay(ParameterStrMap[CCfilterenv], String(env));
        break;
    }
    case CCkeytracking:
        currentPatch.KeyTracking = value;
        groupvec[activeGroupIndex]->setKeytracking(LINEAR[value]);
        if (!setEncValue(CCkeytracking, value, String(LINEAR[value])))
            showCurrentParameterOverlay(ParameterStrMap[CCkeytracking], String(LINEAR[value]));
        break;

    case CCmodwheel:
        // Variable LFO and filter cutoff amount from mod wheel
        groupvec[activeGroupIndex]->setPitchModWheelAmount(POWER[value] * currentPatch.PitchModWheelDepth / 10.0f);
        groupvec[activeGroupIndex]->setFilterModWheelAmount(LINEAR[value] * currentPatch.FilterModWheelDepth * 120.0f);
        break;

    case CCosclfoamt:
        currentPatch.PitchLFOAmt = value;
        groupvec[activeGroupIndex]->setPitchLfoAmount(POWER[value]);
        char buf[10];
        if (!setEncValue(CCosclfoamt, value, dtostrf(POWER[value], 4, 3, buf)))
            showCurrentParameterOverlay(ParameterStrMap[CCosclfoamt], dtostrf(POWER[value], 4, 3, buf));
        break;

    case CCoscLfoRate:
    {
        currentPatch.PitchLFORate = value;
        float rate = 0.0;
        if (groupvec[activeGroupIndex]->getPitchLfoMidiClockSync())
        {
            // TODO: MIDI Tempo stuff remains global?
            currentPatch.LFOTempoValue = LFOTEMPO[value];
            oscLFOTimeDivStr = LFOTEMPOSTR[value];
            rate = currentPatch.LFOSyncFreq * LFOTEMPO[value];
        }
        else
        {
            rate = LFOMAXRATE * POWER[value];
        }

        groupvec[activeGroupIndex]->setPitchLfoRate(rate);
        if (!setEncValue(CCoscLfoRate, value, String(rate) + F(" Hz")))
            showCurrentParameterOverlay(ParameterStrMap[CCoscLfoRate], String(rate) + F(" Hz"));
        break;
    }

    case CCoscLfoWaveform:
        currentPatch.PitchLFOWaveform = value;
        groupvec[activeGroupIndex]->setPitchLfoWaveform(getLFOWaveform(value));
        if (!setEncValue(CCoscLfoWaveform, value, getWaveformStr(getLFOWaveform(value))))
            showCurrentParameterOverlay(ParameterStrMap[CCoscLfoWaveform], getWaveformStr(getLFOWaveform(value)));
        break;

    case CCosclforetrig:
        currentPatch.PitchLFORetrig = value;
        groupvec[activeGroupIndex]->setPitchLfoRetrig(value > 0);
        if (!setEncValue(CCosclforetrig, value, value > 0 ? F("On") : F("Off")))
            showCurrentParameterOverlay(ParameterStrMap[CCosclforetrig], value > 0 ? F("On") : F("Off"));
        break;

    case CCfilterLFOMidiClkSync:
        currentPatch.FilterLFOMidiClkSync = value;
        groupvec[activeGroupIndex]->setFilterLfoMidiClockSync(value > 0);
        if (!setEncValue(temposync, value, value > 0 ? F("On") : F("Off")))
            showCurrentParameterOverlay(ParameterStrMap[temposync], value > 0 ? F("On") : F("Off"));
        break;

    case CCfilterlforate:
    {
        currentPatch.FilterLFORate = value;
        float rate;
        String timeDivStr = "";
        if (groupvec[activeGroupIndex]->getFilterLfoMidiClockSync())
        {
            currentPatch.LFOTempoValue = LFOTEMPO[value];
            rate = currentPatch.LFOSyncFreq * LFOTEMPO[value];
            timeDivStr = LFOTEMPOSTR[value];
        }
        else
        {
            rate = LFOMAXRATE * POWER[value];
        }

        groupvec[activeGroupIndex]->setFilterLfoRate(rate);

        if (timeDivStr.length() > 0)
        {
            if (!setEncValue(CCfilterlforate, value, timeDivStr))
                showCurrentParameterOverlay(F("LFO Time Division"), timeDivStr);
        }
        else
        {
            if (!setEncValue(CCfilterlforate, value, String(rate) + F(" Hz")))
                showCurrentParameterOverlay(ParameterStrMap[CCfilterlforate], String(rate) + F(" Hz"));
        }
        break;
    }

    case CCfilterlfoamt:
    {
        currentPatch.FilterLfoAmt = value;
        float amt = LINEAR[value] * FILTERMODMIXERMAX;
        groupvec[activeGroupIndex]->setFilterLfoAmt(amt);
        if (!setEncValue(CCfilterlfoamt, value, String(amt)))
            showCurrentParameterOverlay(ParameterStrMap[CCfilterlfoamt], String(amt));
        break;
    }
    case CCfilterlfowaveform:
        currentPatch.FilterLFOWaveform = value;
        groupvec[activeGroupIndex]->setFilterLfoWaveform(getLFOWaveform(value));
        if (!setEncValue(CCfilterlfowaveform, value, getWaveformStr(getLFOWaveform(value))))
            showCurrentParameterOverlay(ParameterStrMap[CCfilterlfowaveform], getWaveformStr(getLFOWaveform(value)));
        break;

    case CCfilterlforetrig:
        currentPatch.FilterLFORetrig = value;
        groupvec[activeGroupIndex]->setFilterLfoRetrig(value > 0);
        if (!setEncValue(CCfilterlforetrig, value, value > 0 ? F("On") : F("Off")))
            showCurrentParameterOverlay(ParameterStrMap[CCfilterlforetrig], value > 0 ? F("On") : F("Off"));
        break;

    case CCoscLFOMidiClkSync:
        currentPatch.PitchLFOMidiClkSync = value;
        groupvec[activeGroupIndex]->setPitchLfoMidiClockSync(value > 0);
        if (!setEncValue(CCoscLFOMidiClkSync, value, value > 0 ? F("On") : F("Off")))
            showCurrentParameterOverlay(ParameterStrMap[CCoscLFOMidiClkSync], value > 0 ? F("On") : F("Off"));
        break;

    case CCfilterattack:
        currentPatch.FilterAttack = value;
        groupvec[activeGroupIndex]->setFilterAttack(ENVTIMES[value]);
        if (!setEncValue(CCfilterattack, value, milliToString(ENVTIMES[value])))
            showCurrentParameterOverlay(ParameterStrMap[CCfilterattack], milliToString(ENVTIMES[value]), FILTER_ENV);
        break;

    case CCfilterdecay:
        currentPatch.FilterDecay = value;
        groupvec[activeGroupIndex]->setFilterDecay(ENVTIMES[value]);
        if (!setEncValue(CCfilterdecay, value, milliToString(ENVTIMES[value])))
            showCurrentParameterOverlay(ParameterStrMap[CCfilterdecay], milliToString(ENVTIMES[value]), FILTER_ENV);
        break;

    case CCfiltersustain:
        currentPatch.FilterSustain = value;
        groupvec[activeGroupIndex]->setFilterSustain(LINEAR[value]);
        if (!setEncValue(CCfiltersustain, value, String(LINEAR[value])))
            showCurrentParameterOverlay(ParameterStrMap[CCfiltersustain], String(LINEAR[value]), FILTER_ENV);
        break;

    case CCfilterrelease:
        currentPatch.FilterRelease = value;
        groupvec[activeGroupIndex]->setFilterRelease(ENVTIMES[value]);
        if (!setEncValue(CCfilterrelease, value, milliToString(ENVTIMES[value])))
            showCurrentParameterOverlay(ParameterStrMap[CCfilterrelease], milliToString(ENVTIMES[value]), FILTER_ENV);
        break;

    case CCampattack:
        currentPatch.Attack = value;
        groupvec[activeGroupIndex]->setAmpAttack(ENVTIMES[value]);
        if (!setEncValue(CCampattack, value, milliToString(ENVTIMES[value])))
        {
            showCurrentParameterOverlay(ParameterStrMap[CCampattack], milliToString(ENVTIMES[value]), AMP_ENV);
        }
        break;

    case CCampdecay:
        currentPatch.Decay = value;
        groupvec[activeGroupIndex]->setAmpDecay(ENVTIMES[value]);
        if (!setEncValue(CCampdecay, value, milliToString(ENVTIMES[value])))
            showCurrentParameterOverlay(ParameterStrMap[CCampdecay], milliToString(ENVTIMES[value]), AMP_ENV);
        break;

    case CCampsustain:
        currentPatch.Sustain = value;
        groupvec[activeGroupIndex]->setAmpSustain(LINEAR[value]);
        if (!setEncValue(CCampsustain, value, String(LINEAR[value])))
            showCurrentParameterOverlay(ParameterStrMap[CCampsustain], String(LINEAR[value]), AMP_ENV);
        break;

    case CCamprelease:
        currentPatch.Release = value;
        groupvec[activeGroupIndex]->setAmpRelease(ENVTIMES[value]);
        if (!setEncValue(CCamprelease, value, milliToString(ENVTIMES[value])))
            showCurrentParameterOverlay(ParameterStrMap[CCamprelease], milliToString(ENVTIMES[value]), AMP_ENV);
        break;

    case CCoscfx:
    {
        currentPatch.OscFX = value;
        groupvec[activeGroupIndex]->setOscFX(value);
        if (value == OSCFXXMOD)
        {
            if (!setEncValue(CCoscfx, value, F("X Mod")))
                showCurrentParameterOverlay(ParameterStrMap[CCoscfx], F("X Mod"));
            // Turn osc level controls on
            setEncValue(true, CCoscLevelA, currentPatch.OscLevelA, groupvec[activeGroupIndex]->getOscLevelA(), CCoscLevelA);
            setEncValue(true, CCoscLevelB, currentPatch.OscLevelB, groupvec[activeGroupIndex]->getOscLevelB(), CCoscLevelB);
        }
        else if (value == OSCFXXOR)
        {
            if (!setEncValue(CCoscfx, value, F("XOR Mod")))
                showCurrentParameterOverlay(ParameterStrMap[CCoscfx], F("XOR Mod"));
            // Turn osc level controls off
            setEncInactive(CCoscLevelA);
            setEncInactive(CCoscLevelB);
        }
        else
        {
            if (!setEncValue(CCoscfx, value, F("Off")))
                showCurrentParameterOverlay(ParameterStrMap[CCoscfx], F("Off"));
            // Turn osc level controls off
            setEncInactive(CCoscLevelA);
            setEncInactive(CCoscLevelB);
        }
        break;
    }
    case CCfxamt:
        currentPatch.EffectAmt = value;
        groupvec[activeGroupIndex]->setEffectAmount(ENSEMBLE_LFO[value]);
        if (!setEncValue(CCfxamt, value, String(ENSEMBLE_LFO[value]) + F(" Hz")))
            showCurrentParameterOverlay(ParameterStrMap[CCfxamt], String(ENSEMBLE_LFO[value]) + F(" Hz"));
        break;

    case CCfxmix:
        currentPatch.EffectMix = value;
        groupvec[activeGroupIndex]->setEffectMix(LINEAR[value]);
        if (!setEncValue(CCfxmix, value, String(LINEAR[value])))
            showCurrentParameterOverlay(ParameterStrMap[CCfxmix], String(LINEAR[value]));
        break;

    case CCallnotesoff:
        groupvec[activeGroupIndex]->allNotesOff();
        break;

    case CCvelocitySens:
        currentPatch.VelocitySensitivity = value;
        if (!setEncValue(CCvelocitySens, value, velocityStr[currentPatch.VelocitySensitivity]))
            showCurrentParameterOverlay(ParameterStrMap[CCvelocitySens], velocityStr[currentPatch.VelocitySensitivity]);
        break;
    case CCFilterVelocitySens:
        currentPatch.FilterVelocitySensitivity = value;
        if (!setEncValue(CCFilterVelocitySens, value, velocityStr[currentPatch.FilterVelocitySensitivity]))
            showCurrentParameterOverlay(ParameterStrMap[CCFilterVelocitySens], velocityStr[currentPatch.FilterVelocitySensitivity]);
        break;
    case CCmonomode:
        currentPatch.MonophonicMode = value;
        groupvec[activeGroupIndex]->setMonophonic(value);
        if (!setEncValue(CCmonomode, value, MonophonicStr[value]))
            showCurrentParameterOverlay(ParameterStrMap[CCmonomode], MonophonicStr[value]);
        break;
    case ampenvshape:
        currentPatch.AmpEnvShape = value;
        for (uint8_t i = 0; i < global.maxVoices(); i++)
        {
            global.Oscillators[i].ampEnvelope_.setEnvType(ENVSHAPE[value]);
        }
        if (!setEncValue(ampenvshape, value, EnvShapeStr[value]))
            showCurrentParameterOverlay(ParameterStrMap[ampenvshape], EnvShapeStr[value]);
        break;
    case filterenvshape:
        currentPatch.FilterEnvShape = value;
        for (uint8_t i = 0; i < global.maxVoices(); i++)
        {
            global.Oscillators[i].filterEnvelope_.setEnvType(ENVSHAPE[value]);
        }
        if (!setEncValue(filterenvshape, value, EnvShapeStr[value]))
            showCurrentParameterOverlay(ParameterStrMap[filterenvshape], EnvShapeStr[value]);
        break;
    case glideshape:
        currentPatch.GlideShape = value;
        for (uint8_t i = 0; i < global.maxVoices(); i++)
        {
            global.Oscillators[i].glide_.setMode(glideShape);
        }
        if (!setEncValue(glideshape, value, GlideShapeStr[value]))
            showCurrentParameterOverlay(ParameterStrMap[glideshape], GlideShapeStr[value]);
        break;
    case pitchbendrange:
        currentPatch.PitchBend = value;
        if (!setEncValue(pitchbendrange, value, String(value)))
            showCurrentParameterOverlay(ParameterStrMap[pitchbendrange], String(value));
        break;
    case filtermodwheeldepth:
        currentPatch.FilterModWheelDepth = value;
        groupvec[activeGroupIndex]->setFilterModWheelDepth(currentPatch.FilterModWheelDepth / 10.0f);
        if (!setEncValue(filtermodwheeldepth, value, String(currentPatch.FilterModWheelDepth / 10.0f)))
            showCurrentParameterOverlay(ParameterStrMap[filtermodwheeldepth], String(currentPatch.FilterModWheelDepth / 10.0f));
        break;
    case pitchmodwheeldepth:
        currentPatch.PitchModWheelDepth = value;
        groupvec[activeGroupIndex]->setPitchModWheelDepth(currentPatch.PitchModWheelDepth / 10.0f);
        if (!setEncValue(pitchmodwheeldepth, value, String(currentPatch.PitchModWheelDepth / 10.0f)))
            showCurrentParameterOverlay(ParameterStrMap[pitchmodwheeldepth], String(currentPatch.PitchModWheelDepth / 10.0f));
        break;
    case MIDIThruMode:
        MIDIThru = value;
        storeMidiThru(MIDIThru);
        if (!setEncValue(MIDIThruMode, value, MIDIThruStr[MIDIThru]))
            showCurrentParameterOverlay(ParameterStrMap[MIDIThruMode], MIDIThruStr[MIDIThru]);
        break;
    case MIDIChOut:
    {
        midiOutCh = value;
        storeMidiOutCh(midiOutCh);
        String sOut = F("Off");
        if (midiOutCh > 0)
            sOut = String(midiOutCh);
        if (!setEncValue(MIDIChOut, value, sOut))
            showCurrentParameterOverlay(ParameterStrMap[MIDIChOut], sOut);
    }
    break;
    case MIDIChIn:
    {
        midiChannel = value;
        storeMidiChannel(midiChannel);
        String sIn = F("All");
        if (midiChannel > MIDI_CHANNEL_OMNI)
            sIn = String(midiChannel);
        if (!setEncValue(MIDIChIn, value, sIn))
            showCurrentParameterOverlay(ParameterStrMap[MIDIChIn], sIn);
    }
    break;
    }
}

FLASHMEM void myProgramChange(byte channel, byte program)
{
    // TODO This should check the MIDI channel?
    // if(channel!=midiChannel)return;

    if (state == State::PERFORMANCEPAGE)
    {
        if (program < PERFORMANCES_LIMIT)
        {
            currentPerformanceIndex = program;
            recallPerformance(currentPerformanceIndex + 1);
            setEncodersState(State::PERFORMANCEPAGE);
            Serial.print(F("MIDI Pgm Change Performance:"));
            Serial.println(performances[currentPerformanceIndex]);
        }
    }
    else
    {
        if (program < patches.size())
        {
            currentPatchIndex = program;
            recallPatch(currentBankIndex, patches[currentPatchIndex].patchUID);
            Serial.print(F("MIDI Pgm Change:"));
            Serial.println(patches[currentPatchIndex].patchUID);
        }
    }
}

FLASHMEM void setSeqTimerPeriod(float bpm)
{
    currentSequence.bpm = bpm;
    currentSequence.tempo_us = 15'000'000 / bpm; // beats per bar microseconds
    sequencer_timer.setNextPeriod(currentSequence.tempo_us / 2);
}

FLASHMEM void sequencerStart(boolean cont = false)
{
    // midi_bpm_timer = 0;
    // midi_bpm_counter = 0;
    // _midi_bpm = -1;
    if (!cont)
    {
        currentSequence.step = 0;
        currentSequence.chain_active_step = 0;
    }

    currentSequence.running = true;

    if (!seqMidiSync)
    {
        // noteOnRoutine(); // First note sounds immediately
        sequencer_timer.start();
    }
}

FLASHMEM void sequencerStop()
{
    sequencer_timer.stop();
    groupvec[activeGroupIndex]->allNotesOff();
    currentSequence.running = false;
    currentSequence.recording = false;
    currentSequence.note_in = 0;
    // currentSequence.step = 0;
    // currentSequence.chain_active_step = 0;
}

FLASHMEM void myMIDIClockContinue()
{
    if (!currentSequence.running)
    {
        myMIDIClockStart();
    }
}

FLASHMEM void myMIDIClockStart()
{
    if (!currentSequence.running && !seqMidiSync)
        seqMidiSync = true;
    midiClkCount = 0;
    if (state == State::SEQUENCEPAGE && seqMidiSync)
        sequencerStart();
    // Resync LFOs when MIDI Clock starts.
    // When there's a jump to a different
    // part of a track, such as in a DAW, the DAW must have same
    // rhythmic quantisation as Tempo Div.

    // TODO: Apply to all groupvec[activeGroupIndex]-> Maybe check channel?
    groupvec[activeGroupIndex]->midiClockStart();
    setMIDIClkSignal(true);
}

FLASHMEM void myMIDIClockStop()
{
    if (seqMidiSync)
        sequencerStop();
    seqMidiSync = false;
    setMIDIClkSignal(false);
}

FLASHMEM void myMIDIClock()
{
    // 24ppq
    if (seqMidiSync && currentSequence.running && (midiClkCount + 1) % 3 == 0)
    {
        sequencer();
    }
    // This recalculates the LFO frequencies if the tempo changes (MIDI cLock is 24ppq)
    if (++midiClkCount > 23)
    {
        // TODO: Most of this needs to move into the VoiceGroup

        // setMIDIClkSignal(!getMIDIClkSignal()); //Flash with tempo
        float timeNow = millis();
        midiClkTimeInterval = (timeNow - previousMillis);
        currentPatch.LFOSyncFreq = 1000.0f / midiClkTimeInterval;
        previousMillis = timeNow;
        groupvec[activeGroupIndex]->midiClock(currentPatch.LFOSyncFreq * currentPatch.LFOTempoValue);
        midiClkCount = 0;
    }
}

FLASHMEM void silence()
{
    groupvec[activeGroupIndex]->allNotesOff();
    groupvec[activeGroupIndex]->closeEnvelopes();
}

FLASHMEM void recallPatch(uint8_t bank, long patchUID)
{
    if (patchUID == 0)
        return;
    silence();
    loadPatch(bank, patchUID);
    setCurrentPatchData();
    previousPatchIndex = currentPatchIndex;
    previousBankIndex = currentBankIndex;
    storeLastPatchToEEPROM(currentPatchIndex);
    storeLastBankToEEPROM(currentBankIndex);
    // Reset Midi channels - if coming from performance mode
    midiChannel = getMIDIChannel();
    midiOutCh = getMIDIOutCh();
    MIDIThru = getMidiThru();
    changeMIDIThruMode();
}

FLASHMEM void recallPerformance(uint8_t filename)
{
    silence();
    if (loadPerformance(filename))
    {
        //*** THIS IS HARDCODED TO THE FIRST PATCH AS MULTITIMBRALITY ISN'T FUNCTIONING ***
        if (loadPatch(currentPerformance.patches[0].bankIndex, currentPerformance.patches[0].UID))
        {
            midiChannel = currentPerformance.patches[0].midiCh;
            midiOutCh = currentPerformance.patches[0].midiChOut;
            MIDIThru = currentPerformance.patches[0].midiThru;
            changeMIDIThruMode();

            Serial.println(midiChannel);
            // TODO Set min and max note range but this is only useful for split and layer(multitimbral functionality)
            setCurrentPatchData();
            currentBankIndex = currentPerformance.patches[0].bankIndex;
            loadPatchNamesFromBank(currentBankIndex);
            currentPatchIndex = findPatchIndex(currentPerformance.patches[0].UID);
        }
        else
        {
            // Patch missing
            Serial.print(F("Patch Missing:"));
            Serial.println(currentPerformance.patches[0].UID);
        }
    }
    else
    {
        // Performance missing
        Serial.print(F("Performance Missing:"));
        Serial.println(filename);
    }
}

FLASHMEM void recallSequence(uint8_t filename)
{
    silence();
    if (!loadSequence(filename))
    {
        // Sequence missing
        Serial.print(F("Sequence Missing:"));
        Serial.println(filename);
    }
}

FLASHMEM void setCurrentPatchData()
{
    updatePatchnameAndIndex(currentPatch.PatchName, currentPatchIndex + 1, currentPatch.UID);
    updateDisplay = false; // Don't show each parameter on the display as they are all updated.
    myControlChange(midiChannel, CCoscLevelA, currentPatch.OscLevelA);
    myControlChange(midiChannel, CCoscLevelB, currentPatch.OscLevelB);
    myControlChange(midiChannel, CCnoiseLevel, currentPatch.NoiseLevel);
    myControlChange(midiChannel, CCunison, currentPatch.Unison);
    myControlChange(midiChannel, CCoscfx, currentPatch.OscFX);
    myControlChange(midiChannel, CCdetune, currentPatch.Detune);
    myControlChange(midiChannel, CCoscLfoRate, currentPatch.PitchLFORate);
    myControlChange(midiChannel, CCkeytracking, currentPatch.KeyTracking);
    myControlChange(midiChannel, CCglide, currentPatch.Glide);
    myControlChange(midiChannel, CCpitchA, currentPatch.PitchA);
    myControlChange(midiChannel, CCpitchB, currentPatch.PitchB);
    myControlChange(midiChannel, CCoscwaveformA, currentPatch.WaveformA);
    myControlChange(midiChannel, CCoscwaveformB, currentPatch.WaveformB);
    myControlChange(midiChannel, CCpwmSourceA, currentPatch.PWMSourceA);
    myControlChange(midiChannel, CCpwmSourceB, currentPatch.PWMSourceB);
    myControlChange(midiChannel, CCpwA, currentPatch.PWA_Amount);
    myControlChange(midiChannel, CCpwmAmtA, currentPatch.PWMA_Amount);
    myControlChange(midiChannel, CCpwB, currentPatch.PWB_Amount);
    myControlChange(midiChannel, CCpwmAmtB, currentPatch.PWMB_Amount);
    myControlChange(midiChannel, CCpwmRateA, currentPatch.PWMRateA);
    myControlChange(midiChannel, CCpwmRateB, currentPatch.PWMRateB);
    myControlChange(midiChannel, CCfilterres, currentPatch.FilterRes);
    myControlChange(midiChannel, filterfreq256, currentPatch.FilterFreq); // 8 bit
    myControlChange(midiChannel, CCfiltermixer, currentPatch.FilterMixer);
    myControlChange(midiChannel, CCfilterenv, currentPatch.FilterEnv);
    myControlChange(midiChannel, CCosclfoamt, currentPatch.PitchLFOAmt);
    myControlChange(midiChannel, CCoscLfoRate, currentPatch.PitchLFORate);
    myControlChange(midiChannel, CCoscLfoWaveform, currentPatch.PitchLFOWaveform);
    myControlChange(midiChannel, CCosclforetrig, currentPatch.PitchLFORetrig);
    myControlChange(midiChannel, CCoscLFOMidiClkSync, currentPatch.PitchLFORetrig);
    myControlChange(midiChannel, CCfilterlforate, currentPatch.FilterLFORate);
    myControlChange(midiChannel, CCfilterlforetrig, currentPatch.FilterLFORetrig);
    myControlChange(midiChannel, CCfilterLFOMidiClkSync, currentPatch.FilterLFOMidiClkSync);
    myControlChange(midiChannel, CCfilterlfoamt, currentPatch.FilterLfoAmt);
    myControlChange(midiChannel, CCfilterlfowaveform, currentPatch.FilterLFOWaveform);
    myControlChange(midiChannel, CCfilterattack, currentPatch.FilterAttack);
    myControlChange(midiChannel, CCfilterdecay, currentPatch.FilterDecay);
    myControlChange(midiChannel, CCfiltersustain, currentPatch.FilterSustain);
    myControlChange(midiChannel, CCfilterrelease, currentPatch.FilterRelease);
    myControlChange(midiChannel, CCampattack, currentPatch.Attack);
    myControlChange(midiChannel, CCampdecay, currentPatch.Decay);
    myControlChange(midiChannel, CCampsustain, currentPatch.Sustain);
    myControlChange(midiChannel, CCamprelease, currentPatch.Release);
    myControlChange(midiChannel, CCfxamt, currentPatch.EffectAmt);
    myControlChange(midiChannel, CCfxmix, currentPatch.EffectMix);
    myControlChange(midiChannel, CCpitchenv, currentPatch.PitchEnv);
    myControlChange(midiChannel, CCvelocitySens, currentPatch.VelocitySensitivity);
    myControlChange(midiChannel, pitchmodwheeldepth, currentPatch.PitchModWheelDepth);
    myControlChange(midiChannel, filtermodwheeldepth, currentPatch.FilterModWheelDepth);
    myControlChange(midiChannel, pitchbendrange, currentPatch.PitchBend);
    groupvec[activeGroupIndex]->params().chordDetune = currentPatch.ChordDetune;
    groupvec[activeGroupIndex]->setMonophonic(currentPatch.MonophonicMode);
    updateDisplay = true;

    Serial.print(F("Set Patch: "));
    Serial.print(currentPatch.PatchName);
    Serial.print(F(" UID: "));
    Serial.println(groupvec[activeGroupIndex]->getUID());
}

// Scales the encoder inc/decrement depending on the range to traverse
FLASHMEM int8_t encScaling(EncoderMappingStruct *enc, int8_t delta)
{
    int8_t countLimit = 0;
    switch (enc->Range)
    {
    case 64 ... 127:
        countLimit = 3;
        break;
    case 32 ... 63:
        countLimit = 4;
        break;
    case 16 ... 31:
        countLimit = 5;
        break;
    case 8 ... 15:
        countLimit = 7;
        break;
    case 4 ... 7:
        countLimit = 10;
        break;
    case 1 ... 3:
        countLimit = 20;
        break;
    default:
        countLimit = 1;
        break;
    }

    enc->Counter += delta;
    if (enc->Counter < (countLimit * -1) || enc->Counter > countLimit)
    {
        enc->Counter = 0;
        if ((enc->Value + delta > -1) && (enc->Value + delta < enc->Range + 1))
        {
            enc->Value += delta;
        }
    }
    else
    {
        delta = 0;
    }

    return delta;
}

FLASHMEM void encoderCallback(unsigned enc_idx, int value, int delta)
{
    // --- Value isn't used because it's absolute values that dont change with changes to the encoder assignment---
    // Serial.printf("enc[%u]: v=%d, d=%d\n", enc_idx, value, delta);
    //  Subtract 4 from encoder index due to numbering on shift registers
    enc_idx -= 4;
    if (encMap[enc_idx].active)
    {
        int8_t newDelta = encScaling(&encMap[enc_idx], delta);
        switch (encMap[enc_idx].Parameter)
        {
        case patchselect:
        case namepatch:
            if (!cardStatus)
                return;
            if (newDelta == 0)
                break;
            newDelta > 0 ? incCurrentPatchIndex() : decCurrentPatchIndex();
            if (state == State::MAIN || state == State::DELETEPATCH)
            {
                recallPatch(currentBankIndex, patches[currentPatchIndex].patchUID);
            }
            else if (state == State::PATCHSAVING)
            {
                currentPatchName = patches[currentPatchIndex].patchName;
            }
            else if (state == State::PERFORMANCEPATCHEDIT)
            {
                recallPatch(currentBankIndex, patches[currentPatchIndex].patchUID);
                currentPerformance.patches[0].bankIndex = currentBankIndex;
                currentPerformance.patches[0].UID = patches[currentPatchIndex].patchUID;
                currentPerformance.patches[0].patchName = currentPatchName;
            }
            break;
        case CCbankselectLSB:
            if (!cardStatus)
                return;
            if (newDelta == 0)
                break;
            loadPatchNamesFromBank(newDelta > 0 ? incTempBankIndex() : decTempBankIndex());
            if (state == State::PATCHLIST || state == State::EDITBANK || state == State::DELETEPATCH)
            {
                currentBankIndex = tempBankIndex;
                currentPatchIndex = 0;
            }
            else if (state == State::PERFORMANCEPATCHEDIT)
            {
                currentBankIndex = tempBankIndex;
                currentPatchIndex = patches.size() - 1;
                recallPatch(currentBankIndex, patches[currentPatchIndex].patchUID);
            }
            else
            {
                while (patches[0].patchUID == 0)
                {
                    loadPatchNamesFromBank(newDelta > 0 ? incTempBankIndex() : decTempBankIndex());
                }
                currentBankIndex = tempBankIndex;
                currentPatchIndex = 0;
                recallPatch(currentBankIndex, patches[currentPatchIndex].patchUID);
            }
            break;
        case savebankselect:
            if (!cardStatus)
                return;
            if (newDelta == 0)
                break;
            loadPatchNamesFromBank(newDelta > 0 ? incTempBankIndex() : decTempBankIndex());
            currentBankIndex = tempBankIndex;
            if (patches[0].patchUID != 0)
                patches.push_back(PatchUIDAndName{0, F("-Empty-")});
            currentPatchIndex = patches.size() - 1;
            break;
        case bankeditselect:
            if (!cardStatus)
                return;
            if (newDelta == 0)
                break;
            loadPatchNamesFromBank(newDelta > 0 ? incTempBankIndex() : decTempBankIndex());
            break;
        case choosecharacterPatch:
        case choosecharacterBank:
        case choosecharacterPerformance:
        case choosecharacterSequence:
            charCursor += newDelta;
            if (charCursor >= TOTALCHARS)
                charCursor = 0;
            if (charCursor <= -1)
                charCursor = TOTALCHARS - 1;
            break;
        case deleteCharacterBank:
            if (newDelta > 0 && ++nameCursor > bankNames[currentBankIndex].length() - 1)
            {
                nameCursor = bankNames[currentBankIndex].length() - 1;
            }
            else if (newDelta < 0 && --nameCursor < 0)
            {
                nameCursor = 0;
            }
            break;
        case deleteCharacterPatch:
            if (newDelta > 0 && ++nameCursor > currentPatchName.length() - 1)
            {
                nameCursor = currentPatchName.length() - 1;
            }
            else if (newDelta < 0 && --nameCursor < 0)
            {
                nameCursor = 0;
            }
            break;
        case deleteCharacterPerformance:
            if (newDelta > 0 && ++nameCursor > currentPerformance.performanceName.length() - 1)
            {
                nameCursor = currentPerformance.performanceName.length() - 1;
            }
            else if (newDelta < 0 && --nameCursor < 0)
            {
                nameCursor = 0;
            }
            break;
        case deleteCharacterSequence:
            if (newDelta > 0 && ++nameCursor > currentSequence.SequenceName.length() - 1)
            {
                nameCursor = currentSequence.SequenceName.length() - 1;
            }
            else if (newDelta < 0 && --nameCursor < 0)
            {
                nameCursor = 0;
            }
            break;
        case PerfSelect:
        case PerfEdit:
            if (!cardStatus)
                return;
            if (newDelta == 0)
                break;
            newDelta > 0 ? incPerformanceIndex() : decPerformanceIndex();
            break;
        case SeqSelect:
        case SeqEdit:
            if (!cardStatus)
                return;
            if (newDelta == 0)
                break;
            newDelta > 0 ? incSequenceIndex() : decSequenceIndex();
            break;
        case chooseEncoderTL:
        case chooseEncoderTR:
        case chooseEncoderBL:
        case chooseEncoderBR:
            if (newDelta == 0 || encMap[enc_idx].Value < 0 || encMap[enc_idx].Value > encMap[enc_idx].Range)
                break;
            setEncValue(encMap[enc_idx].Parameter, encMap[enc_idx].Value, ParameterStrMap[ParametersForPerformanceEncoders[encMap[enc_idx].Value]]);
            currentPerformance.TL = ParametersForPerformanceEncoders[encMap[ENC_TL].Value];
            currentPerformance.TR = ParametersForPerformanceEncoders[encMap[ENC_TR].Value];
            currentPerformance.BL = ParametersForPerformanceEncoders[encMap[ENC_BL].Value];
            currentPerformance.BR = ParametersForPerformanceEncoders[encMap[ENC_BR].Value];
            break;
        case chooseMIDIChIn:
        case choosePerfMIDIChIn:
        {
            String sIn = F("All");
            if (encMap[enc_idx].Value > MIDI_CHANNEL_OMNI)
                sIn = String(encMap[enc_idx].Value);
            if (state == State::PERFORMANCEMIDIEDIT)
            {
                currentPerformance.patches[0].midiCh = encMap[enc_idx].Value;
                setEncValue(choosePerfMIDIChIn, encMap[enc_idx].Value, sIn);
            }
            else if (!setEncValue(chooseMIDIChIn, encMap[enc_idx].Value, sIn))
            {
                showCurrentParameterOverlay(ParameterStrMap[chooseMIDIChIn], sIn);
            }
        }
        break;
        case chooseMIDIChOut:
        case choosePerfMIDIChOut:
        {
            String sOut = F("Off");
            if (encMap[enc_idx].Value > 0)
                sOut = String(encMap[enc_idx].Value);
            if (state == State::PERFORMANCEMIDIEDIT)
            {
                currentPerformance.patches[0].midiChOut = encMap[enc_idx].Value;
                setEncValue(choosePerfMIDIChOut, encMap[enc_idx].Value, sOut);
            }
            else if (!setEncValue(chooseMIDIChOut, encMap[enc_idx].Value, sOut))
            {
                showCurrentParameterOverlay(ParameterStrMap[chooseMIDIChOut], sOut);
            }
        }
        break;
        case chooseMIDIThruMode:
        case choosePerfMIDIThruMode:
        {
            if (state == State::PERFORMANCEMIDIEDIT)
            {
                currentPerformance.patches[0].midiThru = encMap[enc_idx].Value;
                setEncValue(choosePerfMIDIThruMode, encMap[enc_idx].Value, MIDIThruStr[encMap[enc_idx].Value]);
            }
            else if (!setEncValue(chooseMIDIThruMode, encMap[enc_idx].Value, MIDIThruStr[encMap[enc_idx].Value]))
            {
                showCurrentParameterOverlay(ParameterStrMap[chooseMIDIThruMode], MIDIThruStr[encMap[enc_idx].Value]);
            }
        }
        break;
        case settingoption:
            if (newDelta == 0)
                break;
            newDelta > 0 ? settings::increment_setting() : settings::decrement_setting();
            setEncValue(settingoption, value, settings::current_setting());
            setEncValue(settingvalue, (int)settings::index(), settings::current_setting_size() - 1, settings::current_setting_value());
            settings::save_current_value();
            break;
        case settingvalue:
            if (newDelta == 0)
                break;
            newDelta > 0 ? settings::increment_setting_value() : settings::decrement_setting_value();
            setEncValue(settingvalue, value, settings::current_setting_value());
            settings::save_current_value();
            break;
        case SeqTempo:
            if (newDelta == 0 || currentSequence.bpm < 20.1 || currentSequence.bpm > 299.9)
                break;
            currentSequence.bpm += (newDelta / 10.0f);
            setSeqTimerPeriod(currentSequence.bpm);
            break;
        default:
            // Serial.printf("enc[%u]: v=%d, d=%d\n", enc_idx, encMap[enc_idx].Value, newDelta);
            myControlChange(midiChannel, encMap[enc_idx].Parameter, encMap[enc_idx].Value);
            midiCCOut(encMap[enc_idx].Parameter, encMap[enc_idx].Value);
            break;
        }
    }
}

FLASHMEM void encoderButtonCallback(unsigned enc_idx, int buttonState)
{
    // Subtract 4 from encoder index due to numbering on shift registers
    enc_idx -= 4;

    // Ignore encoder button held repeatedly
    if (buttonState == HELD_REPEAT)
        return;

    if (encMap[enc_idx].Push && encMap[enc_idx].active)
    {
        switch (encMap[enc_idx].Parameter)
        {
        case patchselect:
        case CCbankselectLSB:
            // If current patch is empty in this bank, go to previous bank with valid patch
            while (patches[0].patchUID == 0)
            {
                tempBankIndex = decTempBankIndex();
                loadPatchNamesFromBank(tempBankIndex);
                currentBankIndex = tempBankIndex;
                currentPatchIndex = 0;
            }
            recallPatch(currentBankIndex, patches[currentPatchIndex].patchUID);
            lightRGLEDs(0, 0);
            break;
        case namepatch:
            if (patches[currentPatchIndex].patchUID == 0)
                currentPatchName = "";
            nameCursor = currentPatchName.length() - 1;
            break;
        case editbank:
            nameCursor = bankNames[tempBankIndex].length() - 1;
            break;
        case cancel:
            currentPatchIndex = previousPatchIndex;
            currentBankIndex = previousBankIndex;
            loadBankNames(); // If in the middle of bank renaming
            loadPatchNamesFromBank(currentBankIndex);
            recallPatch(currentBankIndex, patches[currentPatchIndex].patchUID);
        case goback:
            lightRGLEDs(0, 0);
            break;
        case choosecharacterPatch:
            if (currentPatchName.length() < PATCHNAMEMAXLEN)
            {
                currentPatchName = currentPatchName.substring(0, nameCursor + 1) + CHARACTERS[charCursor] + currentPatchName.substring(nameCursor + 1, currentPatchName.length());
                nameCursor++;
            }
            break;
        case choosecharacterBank:
            if (bankNames[tempBankIndex].length() < BANKNAMEMAXLEN)
                bankNames[tempBankIndex] = bankNames[tempBankIndex].substring(0, nameCursor + 1) + CHARACTERS[charCursor] + bankNames[tempBankIndex].substring(nameCursor + 1, bankNames[tempBankIndex].length());
            break;
        case choosecharacterPerformance:
            if (currentPerformance.performanceName.length() < PATCHNAMEMAXLEN)
            {
                currentPerformance.performanceName = currentPerformance.performanceName.substring(0, nameCursor + 1) + CHARACTERS[charCursor] + currentPerformance.performanceName.substring(nameCursor + 1, currentPerformance.performanceName.length());
                nameCursor++;
            }
            break;
        case deleteCharacterPatch:
            if (buttonState == HELD)
            {
                currentPatchName = "";
                nameCursor = 0;
                break;
            }
            if (currentPatchName.length() == 0)
                break;
            currentPatchName = currentPatchName.substring(0, nameCursor) + currentPatchName.substring(nameCursor + 1, currentPatchName.length());
            nameCursor--;
            break;
        case deleteCharacterBank:
            if (buttonState == HELD)
            {
                bankNames[tempBankIndex] = "";
                nameCursor = 0;
                break;
            }
            if (bankNames[tempBankIndex].length() == 0)
                break;
            nameCursor--;
            bankNames[tempBankIndex] = String(bankNames[tempBankIndex]).substring(0, bankNames[tempBankIndex].length() - 1);
            break;
        case deleteCharacterPerformance:
            if (buttonState == HELD)
            {
                currentPerformance.performanceName = "";
                nameCursor = 0;
                break;
            }
            if (currentPerformance.performanceName.length() == 0)
                break;
            currentPerformance.performanceName = currentPerformance.performanceName.substring(0, nameCursor) +
                                                 currentPerformance.performanceName.substring(nameCursor + 1, currentPerformance.performanceName.length());
            nameCursor--;
            break;
        case savepatch:
            state = State::SAVE;
            strncpy(currentPatch.PatchName, currentPatchName.c_str(), 64);
            savePatch(currentBankIndex, currentPatchIndex);
            updatePatchnameAndIndex(currentPatch.PatchName, currentPatchIndex + 1, currentPatch.UID);
            recallPatch(currentBankIndex, patches[currentPatchIndex].patchUID);
            lightRGLEDs(0, 0);
            break;
        case deletepatch:
            if (patches.size() > 0)
            {
                state = State::DELETEMSG;
                deletePatch(currentBankIndex, currentPatchIndex);
            }
            currentPatchIndex = 0;
            loadPatchNamesFromBank(currentBankIndex);
            recallPatch(currentBankIndex, patches[currentPatchIndex].patchUID);
            lightRGLEDs(0, 0);
            break;
        case savebank:
            state = State::MAIN;
            saveBankName(bankNames[tempBankIndex]);
            while (patches[0].patchUID == 0)
            {
                tempBankIndex = decTempBankIndex();
                loadPatchNamesFromBank(tempBankIndex);
                currentBankIndex = tempBankIndex;
                currentPatchIndex = 0;
            }
            recallPatch(currentBankIndex, patches[currentPatchIndex].patchUID);
            lightRGLEDs(0, 0);
            break;
        case deletebank:
            state = State::DELETEBANKMSG;
            deleteBank(tempBankIndex);
            currentPatchIndex = 0;
            // First Patch is empty in this bank, go to previous bank with valid patch
            while (patches[0].patchUID == 0)
            {
                tempBankIndex = decTempBankIndex();
                loadPatchNamesFromBank(tempBankIndex);
                currentBankIndex = tempBankIndex;
                currentPatchIndex = 0;
            }
            recallPatch(currentBankIndex, patches[currentPatchIndex].patchUID);
            lightRGLEDs(0, 0);
            break;
        case PerfSelect:
        case PerfEdit:
            recallPerformance(currentPerformanceIndex + 1);
            singleLED(ledColour::GREEN, 1); // Led 1 stays green
            break;
        case chooseEncoderBL:
        case chooseEncoderBR:
        case chooseEncoderTL:
        case chooseEncoderTR:
            savePerformance();
            break;
        case SeqSelect:
        case SeqEdit:
            recallSequence(currentSequenceIndex + 1);
            singleLED(ledColour::GREEN, 3); // Led 3 stays green
            break;
        case SeqStartStop:
            if (currentSequence.running)
            {
                sequencerStop();
            }
            else
            {
                sequencerStart();
            }
            singleLED(ledColour::GREEN, 3); // Led 3 stays green
            break;
        case SeqTempo:
            currentSequence.bpm = 120.0f;
            setSeqTimerPeriod(currentSequence.bpm);
            break;
        default:
            setDefaultValue(&encMap[enc_idx]);
            myControlChange(midiChannel, encMap[enc_idx].Parameter, encMap[enc_idx].Value);
            midiCCOut(encMap[enc_idx].Parameter, encMap[enc_idx].Value);
            break;
        }
        state = encMap[enc_idx].PushAction;
        setEncodersState(state);
    }
}

FLASHMEM void buttonCallback(unsigned button_idx, int button)
{
    /*
      Button1 SR1 D4  10
      Button2 SR2 D4  9
      Button3 SR3 D4  11
      Button4 SR1 D5  7
      Button5 SR2 D5  6
      Button6 SR3 D5  8
      Button7 SR1 D6  4
      Button8 SR2 D6  3
      ButtonUp SR1 D7 1
      ButtonDown SR2 D7 0
    */

    switch (button_idx)
    {
    case VOL_UP:
        if (button == HIGH || button == HELD || button == HELD_REPEAT)
        {
            myControlChange(midiChannel, CCvolume, clampToRange<int>(currentVolume, 1, 0, 127));
        }
        break;

    case VOL_DOWN:
        if (button == HIGH || button == HELD || button == HELD_REPEAT)
        {
            myControlChange(midiChannel, CCvolume, clampToRange<int>(currentVolume, -1, 0, 127));
        }
        break;

        // Osc / Multi
    case BUTTON_1:
        if (button == HIGH)
        {
            switch (state)
            {
            case State::OSCPAGE1:
                state = State::OSCPAGE2;
                singleLED(RED, 1);
                break;
            case State::OSCPAGE2:
                state = State::OSCPAGE3;
                singleLED(RED, 1);
                break;
            case State::OSCPAGE3:
                state = State::OSCPAGE1;
                singleLED(RED, 1);
                break;
            case State::PERFORMANCEPAGE:
                state = State::PERFORMANCERECALL;
                singleLED(ledColour::GREEN, 1);
                break;
            case State::PERFORMANCERECALL:
                state = State::MAIN;
                singleLED(ledColour::OFF, 1);
                break;
            case State::PERFORMANCEPATCHEDIT:
                savePerformance();
                state = State::PERFORMANCEENCEDIT;
                singleLED(ledColour::GREEN, 1);
                break;
            case State::PERFORMANCEENCEDIT:
                savePerformance();
                state = State::PERFORMANCEMIDIEDIT;
                singleLED(ledColour::GREEN, 1);
                break;
            case State::PERFORMANCEMIDIEDIT:
                savePerformance();
                state = State::RENAMEPERFORMANCE;
                singleLED(ledColour::GREEN, 1);
                break;
            case State::RENAMEPERFORMANCE:
            case State::CHOOSECHARPERFORMANCE:
            case State::DELETECHARPERFORMANCE:
                savePerformance();
                loadPerformanceNames();
                state = State::PERFORMANCERECALL;
                singleLED(ledColour::GREEN, 1);
                break;
            default:
                state = State::OSCPAGE1; // show osc parameters
                singleLED(RED, 1);
                break;
            }
        }
        if (button == HELD)
        {
            if (state != State::PERFORMANCERECALL && state != State::OSCPAGE1 && state != State::OSCPAGE2 && state != State::OSCPAGE3)
            {
                state = State::PERFORMANCERECALL;
                // Get performances from SD card
                loadPerformanceNames();
                singleLED(GREEN, 1);
            }
            else
            {
                state = State::MAIN;
                singleLED(ledColour::OFF, 1);
            }
        }
        break;

        // Osc Mod / Arp
    case BUTTON_2:
        if (button == HIGH)
        {
            switch (state)
            {
            case State::OSCMODPAGE1:
                state = State::OSCMODPAGE2;
                singleLED(RED, 2);
                break;
            case State::OSCMODPAGE2:
                state = State::OSCMODPAGE3;
                singleLED(RED, 2);
                break;
            case State::OSCMODPAGE3:
                state = State::OSCMODPAGE4;
                singleLED(RED, 2);
                break;
            case State::OSCMODPAGE4:
                state = State::OSCMODPAGE1;
                singleLED(RED, 2);
                break;
            case State::ARPPAGE:
                state = State::MAIN;
                singleLED(ledColour::OFF, 2);
                break;
            case State::PERFORMANCEPAGE:
                state = State::MAIN;
                singleLED(ledColour::OFF, 1);
                break;
            default:
                state = State::OSCMODPAGE1; // show osc mod parameters
                singleLED(RED, 2);
                break;
            }
        }
        if (button == HELD)
        {
            if (state != State::ARPPAGE && state != State::OSCMODPAGE1 && state != State::OSCMODPAGE2 && state != State::OSCMODPAGE3 && state != State::OSCMODPAGE4)
            {
                state = State::ARPPAGE;
                singleLED(GREEN, 2);
            }
            else
            {
                state = State::MAIN;
                singleLED(ledColour::OFF, 2);
            }
        }
        break;

        // Filter / Seq
    case BUTTON_3:
        if (button == HIGH)
        {
            switch (state)
            {
            case State::FILTERPAGE1:
                state = State::FILTERPAGE2;
                singleLED(RED, 3);
                break;
            case State::FILTERPAGE2:
                state = State::FILTERPAGE1;
                singleLED(RED, 3);
                break;
            case State::SEQUENCEPAGE:
                state = State::MAIN;
                singleLED(ledColour::OFF, 3);
                break;
            case State::RENAMESEQUENCE:
            case State::CHOOSECHARSEQUENCE:
            case State::DELETECHARSEQUENCE:
                saveSequence();
                loadSequenceNames();
                state = State::SEQUENCERECALL;
                singleLED(ledColour::GREEN, 3);
                break;
            default:
                state = State::FILTERPAGE1; // show filter parameters
                singleLED(RED, 3);
                break;
            }
        }
        if (button == HELD)
        {
            if (state != State::SEQUENCERECALL && state != State::FILTERPAGE1 && state != State::FILTERPAGE2 && !currentSequence.running)
            {
                state = State::SEQUENCERECALL;
                loadSequenceNames();
                singleLED(GREEN, 3);
            }
            else if (currentSequence.running)
            {
                state = State::SEQUENCEPAGE;
                singleLED(GREEN, 3);
            }
            else
            {
                state = State::MAIN;
                singleLED(ledColour::OFF, 3);
            }
        }
        break;

        // Filter Mod / MIDI
    case BUTTON_4:
        if (button == HIGH)
        {
            switch (state)
            {
            case State::FILTERMODPAGE1:
                state = State::FILTERMODPAGE2;
                singleLED(RED, 4);
                break;
            case State::FILTERMODPAGE2:
                state = State::FILTERMODPAGE1;
                singleLED(RED, 4);
                break;
            case State::MIDIPAGE:
                state = State::MAIN;
                singleLED(ledColour::OFF, 4);
                break;
            case State::PERFORMANCEPAGE:
                state = State::MAIN;
                singleLED(ledColour::OFF, 1);
                break;
            default:
                state = State::FILTERMODPAGE1; // show filter mod parameters
                singleLED(RED, 4);
                break;
            }
        }
        if (button == HELD)
        {
            if (state != State::MIDIPAGE && state != State::FILTERMODPAGE1 && state != State::FILTERMODPAGE2)
            {
                state = State::MIDIPAGE;
                singleLED(GREEN, 4);
            }
            else
            {
                state = State::MAIN;
                singleLED(ledColour::OFF, 4);
            }
        }
        break;
        // Amp / Settings
    case BUTTON_5:
        if (button == HIGH)
        {
            switch (state)
            {
            case State::AMPPAGE1:
                state = State::AMPPAGE2; // show amplifier parameters
                singleLED(RED, 5);
                break;
            case State::AMPPAGE2:
                state = State::AMPPAGE3; // show amplifier parameters
                singleLED(RED, 5);
                break;
            case State::AMPPAGE3:
                state = State::AMPPAGE1; // show amplifier parameters
                singleLED(RED, 5);
                break;
            case State::SETTINGS:
                state = State::MAIN;
                singleLED(ledColour::OFF, 5);
                break;
            case State::PERFORMANCEPAGE:
                state = State::MAIN;
                singleLED(ledColour::OFF, 1);
                break;
            default:
                state = State::AMPPAGE1; // show amplifier parameters
                singleLED(RED, 5);
                break;
            }
        }
        if (button == HELD)
        {
            if (state != State::SETTINGS && state != State::AMPPAGE1 && state != State::AMPPAGE2 && state != State::AMPPAGE3)
            {
                switch (state)
                {
                case State::MAIN:
                    state = SETTINGS;
                    break;
                case SETTINGS:
                    state = State::MAIN;
                    break;
                default:
                    state = SETTINGS;
                    break;
                }
                singleLED(GREEN, 5);
            }
            else
            {
                state = State::MAIN;
                singleLED(ledColour::OFF, 5);
            }
        }
        break;
        // FX / Panic
    case BUTTON_6:
        if (button == HIGH)
        {
            state = State::FXPAGE; // show FX parameters
            singleLED(RED, 6);
        }
        if (button == HELD)
        {

            if (state != State::FXPAGE)
            {
                // If Back button held, Panic - all notes off
                silence();
                flashLED(GREEN, 6, 250);
                state = State::MAIN;
            }
            else
            {
                state = State::MAIN;
                singleLED(ledColour::OFF, 6);
            }
        }
        break;

        // Save / Del
    case BUTTON_7:
        if (button == HIGH)
        {
            // Cancel out of Delete
            if (state == State::DELETEPATCH)
            {
                state = State::MAIN;
                singleLED(ledColour::OFF, 7);
            }
            else if (state == State::PERFORMANCEPAGE)
            {
                state = State::MAIN;
                singleLED(ledColour::OFF, 1);
            }
            // SAVE
            else if (cardStatus && state != State::PATCHSAVING && state != State::CHOOSECHARPATCH)
            {
                singleLED(RED, 7);
                loadBankNames(); // If in the middle of bank renaming and cancelling
                tempBankIndex = currentBankIndex;
                if (patches.size() < PATCHES_LIMIT)
                {
                    // Prompt for patch name
                    currentPatchName = currentPatch.PatchName;
                    currentPatchIndex++;
                    state = State::PATCHSAVING;
                    if (patches[0].patchUID != 0 && patches.back().patchUID != 0)
                        patches.push_back(PatchUIDAndName{0, F("-Empty-")});
                    currentPatchIndex = patches.size() - 1;
                    Serial.printf(F("currentPatchIndex: %i\n"), currentPatchIndex);
                }
                else
                {
                    Serial.println(F("Over patch limit on this bank"));
                }
            }
            else if (state == State::PATCHSAVING)
            {
                // Save without patch naming
                state = State::SAVE;
                savePatch(currentBankIndex, currentPatchIndex);
                updatePatchnameAndIndex(currentPatchName, currentPatchIndex + 1, patches[currentPatchIndex].patchUID);
                recallPatch(currentBankIndex, patches[currentPatchIndex].patchUID);
                state = State::MAIN;
                singleLED(ledColour::OFF, 7);
            }
            else if (state == State::CHOOSECHARPATCH)
            {
                // Save after patch naming
                state = State::SAVE;
                strncpy(currentPatch.PatchName, currentPatchName.c_str(), 64);
                savePatch(currentBankIndex, currentPatchIndex);
                updatePatchnameAndIndex(currentPatch.PatchName, currentPatchIndex + 1, currentPatch.UID);
                recallPatch(currentBankIndex, patches[currentPatchIndex].patchUID);
                state = State::MAIN;
                singleLED(ledColour::OFF, 7);
                break;
            }
            else
            {
                state = State::MAIN;
                singleLED(ledColour::OFF, 7);
            }
        }
        if (button == HELD)
        {
            if (state != State::PATCHSAVING && state != State::DELETEPATCH && state != State::SAVE && state != State::EDITBANK && state != State::RENAMEBANK)
            {
                // DELETE
                state = State::DELETEPATCH;
                loadBankNames(); // If in the middle of bank renaming and cancelling
                tempBankIndex = currentBankIndex;
                singleLED(GREEN, 7);
            }
            else
            {
                state = State::MAIN;
                singleLED(ledColour::OFF, 7);
            }
        }
        break;

        // Recall / Init
    case BUTTON_8:
        if (button == HIGH)
        {
            // RECALL
            singleLED(RED, 8);
            switch (state)
            {
            case State::PATCHLIST:
                // If current patch is empty in this bank, go to previous bank with valid patch
                while (patches[0].patchUID == 0)
                {
                    tempBankIndex = decTempBankIndex();
                    loadPatchNamesFromBank(tempBankIndex);
                    currentBankIndex = tempBankIndex;
                    currentPatchIndex = 0;
                }
                recallPatch(currentBankIndex, patches[currentPatchIndex].patchUID);
                state = State::MAIN;
                singleLED(OFF, 8);
                break;
            default:
                currentPatchIndex = previousPatchIndex; // when coming from cancelled save on last Empty patch
                tempBankIndex = currentBankIndex;
                loadPatchNamesFromBank(tempBankIndex);
                state = State::PATCHLIST; // show patch list
                break;
            }
        }
        if (button == HELD)
        {
            if (state != State::PATCHLIST)
            {
                // Reinitialise to default patch
                state = State::REINITIALISE;
                reinitialisePatch();
                setCurrentPatchData();
                flashLED(GREEN, 8, 300);
                state = State::MAIN;
                lightRGLEDs(currentRLEDs, currentGLEDs);
            }
            else
            {
                state = State::MAIN;
                singleLED(ledColour::OFF, 8);
            }
        }
        break;
    }
    setEncodersState(state);
}

void midiCCOut(byte cc, byte value)
{
    if (midiOutCh > 0 && cc < 129) // Misusing CC for other parameters above 127/128
    {
        if (cc == filterfreq256) // This is CC 128
        {
            // Turn cutoff freq back to 7 bit for MIDI
            cc = CCfilterfreq;
            value = value >> 1;
        }
        usbMIDI.sendControlChange(cc, value, midiOutCh);
        midi1.sendControlChange(cc, value, midiOutCh);
        if (MIDIThru == midi::Thru::Off)
            MIDI.sendControlChange(cc, value, midiOutCh); // MIDI DIN is set to Out
    }
}

FLASHMEM void sdCardDetect()
{
    if (sdCardInterrupt)
    {
        silence();
        delayMicroseconds(100'000);
        cardStatus = !digitalReadFast(pinCD);
        if (cardStatus)
        {
            cardStatus = SD.begin(BUILTIN_SDCARD); // Reinitialise when card inserted
        }
        else
        {
            state = State::MAIN;
            setEncodersState(state);
            lightRGLEDs(0, 0);
        }
        sdCardInterrupt = false;
    }
}

FLASHMEM void usbHostControlChange(byte channel, byte control, byte value)
{
    if (usbHostPluggedIn && usb_host_wait_timer > 4000) // wait 4s
    {
        myControlChange(channel, control, value);
    }
}

FLASHMEM void checkUSBHostStatus()
{
    if (midi1 && !usbHostPluggedIn)
    {
        usb_host_wait_timer = 0;
        usbHostPluggedIn = true;
    }
    else if (!midi1 && usbHostPluggedIn)
    {
        usbHostPluggedIn = false;
    }
}

FLASHMEM void sequencerLEDs()
{
    if (currentSequence.running && currentSequence.step != seq_last_step)
    {
        seq_last_step = currentSequence.step;
        if (currentSequence.step < 8)
        {
            singleLED(RED, currentSequence.step + 1);
        }
        else
        {
            singleLED(GREEN, currentSequence.step - 7);
        }
    }
}

FLASHMEM void CPUMonitor()
{
    Serial.print(F(" CPU:"));
    Serial.print(AudioProcessorUsage());
    Serial.print(F(" ("));
    Serial.print(AudioProcessorUsageMax());
    Serial.print(F(")"));
    Serial.print(F("  MEM:"));
    Serial.println(AudioProcessorUsage());
    // memInfo();
    // getFreeITCM();
    delayMicroseconds(500);
}

void loop()
{
    myusb.Task();
    checkUSBHostStatus();
    // USB Host MIDI Class Compliant
    midi1.read(midiChannel);
    // USB Client MIDI
    usbMIDI.read(midiChannel);
    // MIDI 5 Pin DIN
    MIDI.read(midiChannel);
    encoders.read();
    buttons.read();
    sdCardDetect();
    sequencerLEDs();
    // CPUMonitor();
}