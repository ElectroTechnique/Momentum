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
    Agileware CircularBuffer, Adafruit_GFX (available in Arduino libraries manager)
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
#include "MidiCC.h"

#include "SettingsService.h"
#include "AudioPatching.h"
#include "Constants.h"
#include "Parameters.h"
#include "PatchMgr.h"
#include "HWControls.h"
#include "EepromMgr.h"
#include "Detune.h"
#include "utils.h"
#include "Voice.h"
#include "VoiceGroup.h"

#define PARAMETER 0     // The main page for displaying the current patch and control (parameter) changes
#define RECALL 1        // Patches list
#define SAVE 2          // Save patch page
#define REINITIALISE 3  // Reinitialise message
#define PATCH 4         // Show current patch bypassing PARAMETER
#define PATCHNAMING 5   // Patch naming page
#define DELETE 6        // Delete patch page
#define DELETEMSG 7     // Delete patch message page
#define SETTINGS 8      // Settings page
#define SETTINGSVALUE 9 // Settings page

uint32_t state = PARAMETER;

// Initialize the audio configuration.
Global global{VOICEMIXERLEVEL};
// VoiceGroup voices1{global.SharedAudio[0]};
std::vector<VoiceGroup *> groupvec;
uint8_t activeGroupIndex = 0;

#include "Display.h"

// USB HOST MIDI Class Compliant
USBHost myusb;
USBHub hub1(myusb);
USBHub hub2(myusb);
MIDIDevice midi1(myusb);
// MIDIDevice_BigBuffer midi1(myusb); // Try this if your MIDI Compliant controller has problems

// MIDI 5 Pin DIN - (TRS MIDI)
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

void changeMIDIThruMode()
{
  MIDI.turnThruOn(MIDIThru);
}

#include "Settings.h"

boolean cardStatus = false;
boolean firstPatchLoaded = false;

float previousMillis = millis(); // For MIDI Clk Sync

uint32_t count = 0;           // For MIDI Clk Sync
int voiceToReturn = -1;       // Initialise
long earliestTime = millis(); // For voice allocation - initialise to now

void encoderCallback(unsigned enc_idx, int value, int delta);
void encoderButtonCallback(unsigned button_idx, int state);
void buttonCallback(unsigned button_idx, int state);

FLASHMEM void setup()
{
  while (!Serial)
  {
    delay(10); // TODO Remove
  }
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

  setupDisplay();
  setUpSettings();
  setupHardware(encoderCallback, encoderButtonCallback, buttonCallback);

  AudioMemory(60);

  cardStatus = SD.begin(BUILTIN_SDCARD);
  if (cardStatus)
  {
    Serial.println(F("SD card is connected"));
    // Get patch numbers and names from SD card
    loadPatchNames();
    if (patches.size() == 0)
    {
      // save an initialised patch to SD card using default values in PatchStruct
      savePatch();
      loadPatchNames();
    }
    recallPatch(patches.first().patchUID); // Load first patch from SD card
  }
  else
  {
    Serial.println(F("SD card is not connected or unusable"));
    showPatchPage("No SD", "conn'd / usable");
  }

  // Read MIDI Channel from EEPROM
  midiChannel = getMIDIChannel();
  Serial.println("MIDI In Ch:" + String(midiChannel) + " (0 is Omni On)");

  // USB HOST MIDI Class Compliant
  ledAnimation(70); // Delay to wait to turn on USB Host
  myusb.begin();
  midi1.setHandleControlChange(myControlChange);
  midi1.setHandleNoteOff(myNoteOff);
  midi1.setHandleNoteOn(myNoteOn);
  midi1.setHandlePitchChange(myPitchBend);
  midi1.setHandleProgramChange(myProgramChange);
  midi1.setHandleClock(myMIDIClock);
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
  MIDI.setHandleStart(myMIDIClockStart);
  MIDI.setHandleStop(myMIDIClockStop);
  Serial.println(F("MIDI In DIN Listening"));

  // Read Pitch Bend Range from EEPROM
  pitchBendRange = getPitchBendRange();
  // Read Mod Wheel Depth from EEPROM
  modWheelDepth = getModWheelDepth();
  // Read MIDI Out Channel from EEPROM
  midiOutCh = 1; // getMIDIOutCh();
  // Read MIDI Thru mode from EEPROM
  MIDIThru = getMidiThru();
  changeMIDIThruMode();
  // Read Encoder Direction from EEPROM
  encCW = getEncoderDir();
  // Read oscilloscope enable from EEPROM
  enableScope(getScopeEnable());
  // Read VU enable from EEPROM
  vuMeter = getVUEnable();
  // Read Filter and Amp Envelope shapes
  reloadFiltEnv();
  reloadAmpEnv();
  reloadGlideShape();
}

void myNoteOn(byte channel, byte note, byte velocity)
{
  // Check for out of range notes
  if (note + groupvec[activeGroupIndex]->params().oscPitchA < 0 || note + groupvec[activeGroupIndex]->params().oscPitchA > 127 || note + groupvec[activeGroupIndex]->params().oscPitchB < 0 || note + groupvec[activeGroupIndex]->params().oscPitchB > 127)
    return;

  groupvec[activeGroupIndex]->noteOn(note, velocity);
}

void myNoteOff(byte channel, byte note, byte velocity)
{
  groupvec[activeGroupIndex]->noteOff(note);
}

int getLFOWaveform(int value)
{
  if (value >= 0 && value < 8)
  {
    return WAVEFORM_SINE;
  }
  else if (value >= 8 && value < 30)
  {
    return WAVEFORM_TRIANGLE;
  }
  else if (value >= 30 && value < 63)
  {
    return WAVEFORM_SAWTOOTH_REVERSE;
  }
  else if (value >= 63 && value < 92)
  {
    return WAVEFORM_SAWTOOTH;
  }
  else if (value >= 92 && value < 111)
  {
    return WAVEFORM_SQUARE;
  }
  else
  {
    return WAVEFORM_SAMPLE_HOLD;
  }
}

FLASHMEM String getWaveformStr(int value)
{
  switch (value)
  {
  case WAVEFORM_SILENT:
    return F("Off");
  case WAVEFORM_SAMPLE_HOLD:
    return F("Sample & Hold");
  case WAVEFORM_SINE:
    return F("Sine");
  case WAVEFORM_BANDLIMIT_SQUARE:
  case WAVEFORM_SQUARE:
    return F("Square");
  case WAVEFORM_TRIANGLE:
    return F("Triangle");
  case WAVEFORM_BANDLIMIT_SAWTOOTH:
  case WAVEFORM_SAWTOOTH:
    return F("Sawtooth");
  case WAVEFORM_SAWTOOTH_REVERSE:
    return F("Ramp");
  case WAVEFORM_BANDLIMIT_PULSE:
    return F("Var. Pulse");
  case WAVEFORM_TRIANGLE_VARIABLE:
    return F("Var. Triangle");
  case WAVEFORM_PARABOLIC:
    return F("Parabolic");
  case WAVEFORM_HARMONIC:
    return F("Harmonic");
  default:
    return F("ERR_WAVE");
  }
}

FLASHMEM int getWaveformA(int value)
{
  if (value >= 0 && value < 7)
  {
    // This will turn the osc off
    return WAVEFORM_SILENT;
  }
  else if (value >= 7 && value < 23)
  {
    return WAVEFORM_TRIANGLE;
  }
  else if (value >= 23 && value < 40)
  {
    return WAVEFORM_BANDLIMIT_SQUARE;
  }
  else if (value >= 40 && value < 60)
  {
    return WAVEFORM_BANDLIMIT_SAWTOOTH;
  }
  else if (value >= 60 && value < 80)
  {
    return WAVEFORM_BANDLIMIT_PULSE;
  }
  else if (value >= 80 && value < 100)
  {
    return WAVEFORM_TRIANGLE_VARIABLE;
  }
  else if (value >= 100 && value < 120)
  {
    return WAVEFORM_PARABOLIC;
  }
  else
  {
    return WAVEFORM_HARMONIC;
  }
}

FLASHMEM int getWaveformB(int value)
{
  if (value >= 0 && value < 7)
  {
    // This will turn the osc off
    return WAVEFORM_SILENT;
  }
  else if (value >= 7 && value < 23)
  {
    return WAVEFORM_SAMPLE_HOLD;
  }
  else if (value >= 23 && value < 40)
  {
    return WAVEFORM_BANDLIMIT_SQUARE;
  }
  else if (value >= 40 && value < 60)
  {
    return WAVEFORM_BANDLIMIT_SAWTOOTH;
  }
  else if (value >= 60 && value < 80)
  {
    return WAVEFORM_BANDLIMIT_PULSE;
  }
  else if (value >= 80 && value < 100)
  {
    return WAVEFORM_TRIANGLE_VARIABLE;
  }
  else if (value >= 100 && value < 120)
  {
    return WAVEFORM_PARABOLIC;
  }
  else
  {
    return WAVEFORM_HARMONIC;
  }
}

FLASHMEM void updateUnison(uint8_t unison)
{
  groupvec[activeGroupIndex]->setUnisonMode(unison);

  if (unison == 0)
  {
    showCurrentParameterPage("Unison", "Off");
  }
  else if (unison == 1)
  {
    showCurrentParameterPage("Dyn. Unison", "On");
  }
  else
  {
    showCurrentParameterPage("Chd. Unison", "On");
  }
}

FLASHMEM void updateVolume(float vol)
{
  groupvec[activeGroupIndex]->setVolume(vol * MAX_VOL);
  showCurrentParameterPage("Volume", vol);
}

FLASHMEM void updateGlide(float glideSpeed)
{
  groupvec[activeGroupIndex]->params().glideSpeed = glideSpeed;
  showCurrentParameterPage("Glide", milliToString(glideSpeed * GLIDEFACTOR));
}

FLASHMEM void updateWaveformA(uint32_t waveform)
{
  groupvec[activeGroupIndex]->setWaveformA(waveform);
  showCurrentParameterPage("1. Waveform", getWaveformStr(waveform));
}

FLASHMEM void updateWaveformB(uint32_t waveform)
{
  groupvec[activeGroupIndex]->setWaveformB(waveform);
  showCurrentParameterPage("2. Waveform", getWaveformStr(waveform));
}

FLASHMEM void updatePitchA(int pitch)
{
  groupvec[activeGroupIndex]->params().oscPitchA = pitch;
  groupvec[activeGroupIndex]->updateVoices();
  showCurrentParameterPage("1. Semitones", (pitch > 0 ? "+" : "") + String(pitch));
}

FLASHMEM void updatePitchB(int pitch)
{
  groupvec[activeGroupIndex]->params().oscPitchB = pitch;
  groupvec[activeGroupIndex]->updateVoices();
  showCurrentParameterPage("2. Semitones", (pitch > 0 ? "+" : "") + String(pitch));
}

FLASHMEM void updateDetune(float detune, uint32_t chordDetune)
{
  groupvec[activeGroupIndex]->params().detune = detune;
  groupvec[activeGroupIndex]->params().chordDetune = chordDetune;
  groupvec[activeGroupIndex]->updateVoices();

  if (groupvec[activeGroupIndex]->params().unisonMode == 2)
  {
    showCurrentParameterPage("Chord", CDT_STR[chordDetune]);
  }
  else
  {
    showCurrentParameterPage("Detune", String((1 - detune) * 100) + " %");
  }
}

FLASHMEM void updatePWMSource(uint8_t source)
{
  groupvec[activeGroupIndex]->setPWMSource(source);

  if (source == PWMSOURCELFO)
  {
    showCurrentParameterPage("PWM Source", "LFO"); // Only shown when updated via MIDI
  }
  else
  {
    showCurrentParameterPage("PWM Source", "Filter Env");
  }
}

FLASHMEM void updatePWMRate(float value)
{
  groupvec[activeGroupIndex]->setPwmRate(value);

  if (value == PWMRATE_PW_MODE)
  {
    // Set to fixed PW mode
    showCurrentParameterPage("PW Mode", "On");
  }
  else if (value == PWMRATE_SOURCE_FILTER_ENV)
  {
    // Set to Filter Env Mod source
    showCurrentParameterPage("PWM Source", "Filter Env");
  }
  else
  {
    showCurrentParameterPage("PWM Rate", String(2 * value) + " Hz"); // PWM goes through mid to maximum, sounding effectively twice as fast
  }
}

FLASHMEM void updatePWMAmount(float value)
{
  // MIDI only - sets both osc PWM
  groupvec[activeGroupIndex]->overridePwmAmount(value);
  showCurrentParameterPage("PWM Amt", String(value) + " : " + String(value));
}

FLASHMEM void updatePWA(float valuePwA, float valuePwmAmtA)
{
  groupvec[activeGroupIndex]->setPWA(valuePwA, valuePwmAmtA);

  if (groupvec[activeGroupIndex]->getPwmRate() == PWMRATE_PW_MODE)
  {
    if (groupvec[activeGroupIndex]->getWaveformA() == WAVEFORM_TRIANGLE_VARIABLE)
    {
      showCurrentParameterPage("1. PW Amt", groupvec[activeGroupIndex]->getPwA(), VAR_TRI);
    }
    else
    {
      showCurrentParameterPage("1. PW Amt", groupvec[activeGroupIndex]->getPwA(), PULSE);
    }
  }
  else
  {
    if (groupvec[activeGroupIndex]->getPwmSource() == PWMSOURCELFO)
    {
      // PW alters PWM LFO amount for waveform A
      showCurrentParameterPage("1. PWM Amt", "LFO " + String(groupvec[activeGroupIndex]->getPwmAmtA()));
    }
    else
    {
      // PW alters PWM Filter Env amount for waveform A
      showCurrentParameterPage("1. PWM Amt", "F. Env " + String(groupvec[activeGroupIndex]->getPwmAmtA()));
    }
  }
}

FLASHMEM void updatePWB(float valuePwB, float valuePwmAmtB)
{
  groupvec[activeGroupIndex]->setPWB(valuePwB, valuePwmAmtB);

  if (groupvec[activeGroupIndex]->getPwmRate() == PWMRATE_PW_MODE)
  {
    if (groupvec[activeGroupIndex]->getWaveformB() == WAVEFORM_TRIANGLE_VARIABLE)
    {
      showCurrentParameterPage("2. PW Amt", groupvec[activeGroupIndex]->getPwB(), VAR_TRI);
    }
    else
    {
      showCurrentParameterPage("2. PW Amt", groupvec[activeGroupIndex]->getPwB(), PULSE);
    }
  }
  else
  {
    if (groupvec[activeGroupIndex]->getPwmSource() == PWMSOURCELFO)
    {
      // PW alters PWM LFO amount for waveform B
      showCurrentParameterPage("2. PWM Amt", "LFO " + String(groupvec[activeGroupIndex]->getPwmAmtB()));
    }
    else
    {
      // PW alters PWM Filter Env amount for waveform B
      showCurrentParameterPage("2. PWM Amt", "F. Env " + String(groupvec[activeGroupIndex]->getPwmAmtB()));
    }
  }
}

FLASHMEM void updateOscLevelA(float value)
{
  groupvec[activeGroupIndex]->setOscLevelA(value);

  switch (groupvec[activeGroupIndex]->getOscFX())
  {
  case 1: // XOR
    showCurrentParameterPage("Osc Mix 1:2", "   " + String(groupvec[activeGroupIndex]->getOscLevelA()) + " : " + String(groupvec[activeGroupIndex]->getOscLevelB()));
    break;
  case 2: // XMod
    // osc A sounds with increasing osc B mod
    if (groupvec[activeGroupIndex]->getOscLevelA() == 1.0f && groupvec[activeGroupIndex]->getOscLevelB() <= 1.0f)
    {
      showCurrentParameterPage("XMod Osc 1", "Osc 2: " + String(1 - groupvec[activeGroupIndex]->getOscLevelB()));
    }
    break;
  case 0: // None
    showCurrentParameterPage("Osc Mix 1:2", "   " + String(groupvec[activeGroupIndex]->getOscLevelA()) + " : " + String(groupvec[activeGroupIndex]->getOscLevelB()));
    break;
  }
}

FLASHMEM void updateOscLevelB(float value)
{
  groupvec[activeGroupIndex]->setOscLevelB(value);

  switch (groupvec[activeGroupIndex]->getOscFX())
  {
  case 1: // XOR
    showCurrentParameterPage("Osc Mix 1:2", "   " + String(groupvec[activeGroupIndex]->getOscLevelA()) + " : " + String(groupvec[activeGroupIndex]->getOscLevelB()));
    break;
  case 2: // XMod
    // osc B sounds with increasing osc A mod
    if (groupvec[activeGroupIndex]->getOscLevelB() == 1.0f && groupvec[activeGroupIndex]->getOscLevelA() < 1.0f)
    {
      showCurrentParameterPage("XMod Osc 2", "Osc 1: " + String(1 - groupvec[activeGroupIndex]->getOscLevelA()));
    }
    break;
  case 0: // None
    showCurrentParameterPage("Osc Mix 1:2", "   " + String(groupvec[activeGroupIndex]->getOscLevelA()) + " : " + String(groupvec[activeGroupIndex]->getOscLevelB()));
    break;
  }
}

FLASHMEM void updateNoiseLevel(float value)
{
  float pink = 0.0;
  float white = 0.0;
  if (value > 0)
  {
    pink = value;
  }
  else if (value < 0)
  {
    white = abs(value);
  }

  groupvec[activeGroupIndex]->setPinkNoiseLevel(pink);
  groupvec[activeGroupIndex]->setWhiteNoiseLevel(white);

  if (value > 0)
  {
    showCurrentParameterPage("Noise Level", "Pink " + String(value));
  }
  else if (value < 0)
  {
    showCurrentParameterPage("Noise Level", "White " + String(abs(value)));
  }
  else
  {
    showCurrentParameterPage("Noise Level", "Off");
  }
}

FLASHMEM void updateFilterFreq(float value)
{
  groupvec[activeGroupIndex]->setCutoff(value);
  showCurrentParameterPage("Cutoff", String(int(value)) + " Hz");
}

FLASHMEM void updateFilterRes(float value)
{
  groupvec[activeGroupIndex]->setResonance(value);
  showCurrentParameterPage("Resonance", value);
}

FLASHMEM void updateFilterMixer(float value)
{
  groupvec[activeGroupIndex]->setFilterMixer(value);

  String filterStr;
  if (value == BANDPASS)
  {
    filterStr = "Band Pass";
  }
  else
  {
    // LP-HP mix mode - a notch filter
    if (value == LOWPASS)
    {
      filterStr = "Low Pass";
    }
    else if (value == HIGHPASS)
    {
      filterStr = "High Pass";
    }
    else
    {
      filterStr = "LP " + String(100 - int(100 * value)) + " - " + String(int(100 * value)) + " HP";
    }
  }

  showCurrentParameterPage("Filter Type", filterStr);
}

FLASHMEM void updateFilterEnv(float value)
{
  groupvec[activeGroupIndex]->setFilterEnvelope(value);
  showCurrentParameterPage("Filter Env.", String(value));
}

FLASHMEM void updatePitchEnv(float value)
{
  groupvec[activeGroupIndex]->setPitchEnvelope(value);
  showCurrentParameterPage("Pitch Env Amt", String(value));
}

FLASHMEM void updateKeyTracking(float value)
{
  groupvec[activeGroupIndex]->setKeytracking(value);
  showCurrentParameterPage("Key Tracking", String(value));
}

FLASHMEM void updatePitchLFOAmt(float value)
{
  groupvec[activeGroupIndex]->setPitchLfoAmount(value);
  char buf[10];
  showCurrentParameterPage("LFO Amount", dtostrf(value, 4, 3, buf));
}

FLASHMEM void updateModWheel(float value)
{
  groupvec[activeGroupIndex]->setModWhAmount(value);
}

FLASHMEM void updatePitchLFORate(float value)
{
  groupvec[activeGroupIndex]->setPitchLfoRate(value);
  showCurrentParameterPage("LFO Rate", String(value) + " Hz");
}

FLASHMEM void updatePitchLFOWaveform(uint32_t waveform)
{
  groupvec[activeGroupIndex]->setPitchLfoWaveform(waveform);
  showCurrentParameterPage("Pitch LFO", getWaveformStr(waveform));
}

// MIDI CC only
FLASHMEM void updatePitchLFOMidiClkSync(bool value)
{
  groupvec[activeGroupIndex]->setPitchLfoMidiClockSync(value);
  showCurrentParameterPage("P. LFO Sync", value ? "On" : "Off");
}

FLASHMEM void updateFilterLfoRate(float value, String timeDivStr)
{
  groupvec[activeGroupIndex]->setFilterLfoRate(value);

  if (timeDivStr.length() > 0)
  {
    showCurrentParameterPage("LFO Time Div", timeDivStr);
  }
  else
  {
    showCurrentParameterPage("F. LFO Rate", String(value) + " Hz");
  }
}

FLASHMEM void updateFilterLfoAmt(float value)
{
  groupvec[activeGroupIndex]->setFilterLfoAmt(value);
  showCurrentParameterPage("F. LFO Amt", String(value));
}

FLASHMEM void updateFilterLFOWaveform(uint32_t waveform)
{
  groupvec[activeGroupIndex]->setFilterLfoWaveform(waveform);
  showCurrentParameterPage("Filter LFO", getWaveformStr(waveform));
}

FLASHMEM void updatePitchLFORetrig(bool value)
{
  groupvec[activeGroupIndex]->setPitchLfoRetrig(value);
  showCurrentParameterPage("P. LFO Retrig", value ? "On" : "Off");
}

FLASHMEM void updateFilterLFORetrig(bool value)
{
  groupvec[activeGroupIndex]->setFilterLfoRetrig(value);
  showCurrentParameterPage("F. LFO Retrig", groupvec[activeGroupIndex]->getFilterLfoRetrig() ? "On" : "Off");
}

FLASHMEM void updateFilterLFOMidiClkSync(bool value)
{
  groupvec[activeGroupIndex]->setFilterLfoMidiClockSync(value);
  showCurrentParameterPage("Tempo Sync", value ? "On" : "Off");
}

FLASHMEM void updateFilterAttack(float value)
{
  groupvec[activeGroupIndex]->setFilterAttack(value);
  showCurrentParameterPage("Filter Attack", milliToString(value), FILTER_ENV);
}

FLASHMEM void updateFilterDecay(float value)
{
  groupvec[activeGroupIndex]->setFilterDecay(value);
  showCurrentParameterPage("Filter Decay", milliToString(value), FILTER_ENV);
}

FLASHMEM void updateFilterSustain(float value)
{
  groupvec[activeGroupIndex]->setFilterSustain(value);
  showCurrentParameterPage("Filter Sustain", String(value), FILTER_ENV);
}

FLASHMEM void updateFilterRelease(float value)
{
  groupvec[activeGroupIndex]->setFilterRelease(value);
  showCurrentParameterPage("Filter Release", milliToString(value), FILTER_ENV);
}

FLASHMEM void updateAttack(float value)
{
  groupvec[activeGroupIndex]->setAmpAttack(value);
  showCurrentParameterPage("Attack", milliToString(value), AMP_ENV);
}

FLASHMEM void updateDecay(float value)
{
  groupvec[activeGroupIndex]->setAmpDecay(value);
  showCurrentParameterPage("Decay", milliToString(value), AMP_ENV);
}

FLASHMEM void updateSustain(float value)
{
  groupvec[activeGroupIndex]->setAmpSustain(value);
  showCurrentParameterPage("Sustain", String(value), AMP_ENV);
}

FLASHMEM void updateRelease(float value)
{
  groupvec[activeGroupIndex]->setAmpRelease(value);
  showCurrentParameterPage("Release", milliToString(value), AMP_ENV);
}

FLASHMEM void updateOscFX(uint8_t value)
{
  groupvec[activeGroupIndex]->setOscFX(value);
  if (value == 2)
  {
    showCurrentParameterPage("Osc FX", "On - X Mod");
  }
  else if (value == 1)
  {
    showCurrentParameterPage("Osc FX", "On - XOR");
  }
  else
  {
    showCurrentParameterPage("Osc FX", "Off");
  }
}

FLASHMEM void updateEffectAmt(float value)
{
  groupvec[activeGroupIndex]->setEffectAmount(value);
  showCurrentParameterPage("Effect Amt", String(value) + " Hz");
}

FLASHMEM void updateEffectMix(float value)
{
  groupvec[activeGroupIndex]->setEffectMix(value);
  showCurrentParameterPage("Effect Mix", String(value));
}

FLASHMEM void updatePatch(String name, uint32_t index, uint32_t UID)
{
  groupvec[activeGroupIndex]->setPatchName(name);
  groupvec[activeGroupIndex]->setPatchIndex(index);
  groupvec[activeGroupIndex]->setUID(UID);
  showPatchPage(String(index), name);
}

void myPitchBend(byte channel, int bend)
{
  // 0.5 to give 1oct max - spread of mod is 2oct
  groupvec[activeGroupIndex]->pitchBend(bend * 0.5f * pitchBendRange * DIV12 * DIV8192);
}

// MIDI CC
void myControlChange(byte channel, byte control, byte value)
{
  switch (control)
  {
  case CCvolume:
    updateVolume(LINEAR[value]);
    break;
  case CCunison:
    updateUnison(inRangeOrDefault<int>(value, 2, 0, 2));
    break;

  case CCglide:
    updateGlide(POWER[value]);
    break;

  case CCpitchenv:
    updatePitchEnv(LINEARCENTREZERO[value] * OSCMODMIXERMAX);
    break;

  case CCoscwaveformA:
    updateWaveformA(getWaveformA(value));
    break;

  case CCoscwaveformB:
    updateWaveformB(getWaveformB(value));
    break;

  case CCpitchA:
    updatePitchA(PITCH[value]);
    break;

  case CCpitchB:
    updatePitchB(PITCH[value]);
    break;

  case CCdetune:
    updateDetune(1.0f - (MAXDETUNE * POWER[value]), value);
    break;

  case CCpwmSource:
    updatePWMSource(value > 0 ? PWMSOURCEFENV : PWMSOURCELFO);
    break;

  case CCpwmRate:
    // Uses combination of PWMRate, PWa and PWb
    updatePWMRate(PWMRATE[value]);
    break;

  case CCpwmAmt:
    // NO FRONT PANEL CONTROL - MIDI CC ONLY
    // Total PWM amount for both oscillators
    updatePWMAmount(LINEAR[value]);
    break;

  case CCpwmAmtA:
    // Need to set both PW amount and PWM amount due to a choice of PWM source switching between them
    updatePWA(groupvec[activeGroupIndex]->getPwA(), LINEAR[value]);
    break;

  case CCpwmAmtB:
    // Need to set both PW amount and PWM amount due to a choice of PWM source switching between them
    updatePWB(groupvec[activeGroupIndex]->getPwB(), LINEAR[value]);
    break;

  case CCpwA:
    updatePWA(LINEARCENTREZERO[value], LINEAR[value]);
    break;

  case CCpwB:
    updatePWB(LINEARCENTREZERO[value], LINEAR[value]);
    break;

  case CCoscLevelA:
    updateOscLevelA(LINEAR[value]);
    break;

  case CCoscLevelB:
    updateOscLevelB(LINEAR[value]);
    break;

  case CCnoiseLevel:
    updateNoiseLevel(LINEARCENTREZERO[value]);
    break;

  case CCfilterfreq:
    // MIDI is 7 bit, 128 values and needs to choose alternate filterfreqs(8 bit) by multiplying by 2
    updateFilterFreq(FILTERFREQS256[value * 2]);
    break;

  case filterfreq256:
    // 8 bit from panel controls for smoothness
    updateFilterFreq(FILTERFREQS256[value]);
    break;

  case CCfilterres:
    // If <1.1 there is noise at high cutoff freq
    updateFilterRes((14.29f * POWER[value]) + 0.71f);
    break;

  case CCfiltermixer:
    updateFilterMixer(LINEAR_FILTERMIXER[value]);
    break;

  case CCfilterenv:
    updateFilterEnv(LINEARCENTREZERO[value] * FILTERMODMIXERMAX);
    break;

  case CCkeytracking:
    updateKeyTracking(KEYTRACKINGAMT[value]);
    break;

  case CCmodwheel:
    // Variable LFO amount from mod wheel - Settings Option
    updateModWheel(POWER[value] * modWheelDepth);
    break;

  case CCosclfoamt:
    updatePitchLFOAmt(POWER[value]);
    break;

  case CCoscLfoRate:
  {
    float rate = 0.0;
    if (groupvec[activeGroupIndex]->getPitchLfoMidiClockSync())
    {
      // TODO: MIDI Tempo stuff remains global?
      lfoTempoValue = LFOTEMPO[value];
      oscLFOTimeDivStr = LFOTEMPOSTR[value];
      rate = lfoSyncFreq * LFOTEMPO[value];
    }
    else
    {
      rate = LFOMAXRATE * POWER[value];
    }
    updatePitchLFORate(rate);
    break;
  }

  case CCoscLfoWaveform:
    updatePitchLFOWaveform(getLFOWaveform(value));
    break;

  case CCosclforetrig:
    updatePitchLFORetrig(value > 0);
    break;

  case CCfilterLFOMidiClkSync:
    updateFilterLFOMidiClkSync(value > 0);
    break;

  case CCfilterlforate:
  {
    float rate;
    String timeDivStr = "";
    if (groupvec[activeGroupIndex]->getFilterLfoMidiClockSync())
    {
      lfoTempoValue = LFOTEMPO[value];
      rate = lfoSyncFreq * LFOTEMPO[value];
      timeDivStr = LFOTEMPOSTR[value];
    }
    else
    {
      rate = LFOMAXRATE * POWER[value];
    }

    updateFilterLfoRate(rate, timeDivStr);
    break;
  }

  case CCfilterlfoamt:
    updateFilterLfoAmt(LINEAR[value] * FILTERMODMIXERMAX);
    break;

  case CCfilterlfowaveform:
    updateFilterLFOWaveform(getLFOWaveform(value));
    break;

  case CCfilterlforetrig:
    updateFilterLFORetrig(value > 0);
    break;

  case CCoscLFOMidiClkSync:
    updatePitchLFOMidiClkSync(value > 0);
    break;

  case CCfilterattack:
    updateFilterAttack(ENVTIMES[value]);
    break;

  case CCfilterdecay:
    updateFilterDecay(ENVTIMES[value]);
    break;

  case CCfiltersustain:
    updateFilterSustain(LINEAR[value]);
    break;

  case CCfilterrelease:
    updateFilterRelease(ENVTIMES[value]);
    break;

  case CCampattack:
    updateAttack(ENVTIMES[value]);
    break;

  case CCampdecay:
    updateDecay(ENVTIMES[value]);
    break;

  case CCampsustain:
    updateSustain(LINEAR[value]);
    break;

  case CCamprelease:
    updateRelease(ENVTIMES[value]);
    break;

  case CCoscfx:
    updateOscFX(inRangeOrDefault<int>(value, 2, 0, 2));
    break;

  case CCfxamt:
    updateEffectAmt(ENSEMBLE_LFO[value]);
    break;

  case CCfxmix:
    updateEffectMix(LINEAR[value]);
    break;

  case CCallnotesoff:
    groupvec[activeGroupIndex]->allNotesOff();
    break;
  }
}

FLASHMEM void myProgramChange(byte channel, byte program)
{
  state = PATCH;
  recallPatch(patches[program].patchUID);
  Serial.print(F("MIDI Pgm Change:"));
  Serial.println(patches[program].patchUID);
  state = PARAMETER;
}

FLASHMEM void myMIDIClockStart()
{
  setMIDIClkSignal(true);
  // Resync LFOs when MIDI Clock starts.
  // When there's a jump to a different
  // part of a track, such as in a DAW, the DAW must have same
  // rhythmic quantisation as Tempo Div.

  // TODO: Apply to all groupvec[activeGroupIndex]-> Maybe check channel?
  groupvec[activeGroupIndex]->midiClockStart();
}

FLASHMEM void myMIDIClockStop()
{
  setMIDIClkSignal(false);
}

FLASHMEM void myMIDIClock()
{
  // This recalculates the LFO frequencies if the tempo changes (MIDI cLock is 24ppq)
  if (count > 23)
  {
    // TODO: Most of this needs to move into the VoiceGroup

    setMIDIClkSignal(!getMIDIClkSignal());
    float timeNow = millis();
    midiClkTimeInterval = (timeNow - previousMillis);
    lfoSyncFreq = 1000.0f / midiClkTimeInterval;
    previousMillis = timeNow;
    groupvec[activeGroupIndex]->midiClock(lfoSyncFreq * lfoTempoValue);
    count = 0;
  }

  count++;
}

FLASHMEM void recallPatch(long patchUID)
{
  groupvec[activeGroupIndex]->allNotesOff();
  groupvec[activeGroupIndex]->closeEnvelopes();
  loadPatch(patchUID);
  setCurrentPatchData();
}

FLASHMEM void setCurrentPatchData()
{
  updatePatch(currentPatch.PatchName, 0, currentPatch.UID); // TODO remove patchno
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
  myControlChange(midiChannel, CCpwmSource, currentPatch.PWMSource);
  myControlChange(midiChannel, CCpwA, currentPatch.PWA_Amount);
  myControlChange(midiChannel, CCpwmAmtA, currentPatch.PWMA_Amount);
  myControlChange(midiChannel, CCpwB, currentPatch.PWB_Amount);
  myControlChange(midiChannel, CCpwmAmtB, currentPatch.PWMB_Amount);
  myControlChange(midiChannel, CCpwmRate, currentPatch.PWMRate);
  myControlChange(midiChannel, CCfilterres, currentPatch.FilterRes);
  myControlChange(midiChannel, CCfilterfreq, currentPatch.FilterFreq);
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
  velocitySens = currentPatch.VelocitySensitivity;
  groupvec[activeGroupIndex]->params().chordDetune = currentPatch.ChordDetune;
  groupvec[activeGroupIndex]->setMonophonic(currentPatch.MonophonicMode);

  Serial.print(F("Set Patch: "));
  Serial.print(currentPatch.PatchName);
  Serial.print(F(" UID: "));
  Serial.println(groupvec[activeGroupIndex]->getUID()); // TODO remove
}

FLASHMEM String getCurrentPatchData()
{
  // Add the UID to end
  String pd = getCurrentPatchDataWithoutPatchname();
  char arr[pd.length() + 1];
  strcpy(arr, pd.c_str());
  return patchName + "," + arr; // + getHash(arr);
}

FLASHMEM String getCurrentPatchDataWithoutPatchname()
{

  auto p = groupvec[activeGroupIndex]->params();
  return String(groupvec[activeGroupIndex]->getOscLevelA()) + "," + String(groupvec[activeGroupIndex]->getOscLevelB()) + "," + String(groupvec[activeGroupIndex]->getPinkNoiseLevel() - groupvec[activeGroupIndex]->getWhiteNoiseLevel()) + "," + String(p.unisonMode) + "," + String(groupvec[activeGroupIndex]->getOscFX()) + "," + String(p.detune, 5) + "," + String(lfoSyncFreq) + "," + String(midiClkTimeInterval) + "," + String(lfoTempoValue) + "," + String(groupvec[activeGroupIndex]->getKeytrackingAmount()) + "," + String(p.glideSpeed, 5) + "," + String(p.oscPitchA) + "," + String(p.oscPitchB) + "," + String(groupvec[activeGroupIndex]->getWaveformA()) + "," + String(groupvec[activeGroupIndex]->getWaveformB()) + "," +
         String(groupvec[activeGroupIndex]->getPwmSource()) + "," + String(groupvec[activeGroupIndex]->getPwmAmtA()) + "," + String(groupvec[activeGroupIndex]->getPwmAmtB()) + "," + String(groupvec[activeGroupIndex]->getPwmRate()) + "," + String(groupvec[activeGroupIndex]->getPwA()) + "," + String(groupvec[activeGroupIndex]->getPwB()) + "," + String(groupvec[activeGroupIndex]->getResonance()) + "," + String(groupvec[activeGroupIndex]->getCutoff()) + "," + String(groupvec[activeGroupIndex]->getFilterMixer()) + "," + String(groupvec[activeGroupIndex]->getFilterEnvelope()) + "," + String(groupvec[activeGroupIndex]->getPitchLfoAmount(), 5) + "," + String(groupvec[activeGroupIndex]->getPitchLfoRate(), 5) + "," + String(groupvec[activeGroupIndex]->getPitchLfoWaveform()) + "," + String(int(groupvec[activeGroupIndex]->getPitchLfoRetrig())) + "," + String(int(groupvec[activeGroupIndex]->getPitchLfoMidiClockSync())) + "," + String(groupvec[activeGroupIndex]->getFilterLfoRate(), 5) + "," +
         groupvec[activeGroupIndex]->getFilterLfoRetrig() + "," + groupvec[activeGroupIndex]->getFilterLfoMidiClockSync() + "," + groupvec[activeGroupIndex]->getFilterLfoAmt() + "," + groupvec[activeGroupIndex]->getFilterLfoWaveform() + "," + groupvec[activeGroupIndex]->getFilterAttack() + "," + groupvec[activeGroupIndex]->getFilterDecay() + "," + groupvec[activeGroupIndex]->getFilterSustain() + "," + groupvec[activeGroupIndex]->getFilterRelease() + "," + groupvec[activeGroupIndex]->getAmpAttack() + "," + groupvec[activeGroupIndex]->getAmpDecay() + "," + groupvec[activeGroupIndex]->getAmpSustain() + "," + groupvec[activeGroupIndex]->getAmpRelease() + "," +
         String(groupvec[activeGroupIndex]->getEffectAmount()) + "," + String(groupvec[activeGroupIndex]->getEffectMix()) + "," + String(groupvec[activeGroupIndex]->getPitchEnvelope()) + "," + String(velocitySens) + "," + String(p.chordDetune) + "," + String(groupvec[activeGroupIndex]->getMonophonicMode()) + "," + String(0.0f);
}

/*
  switch (p) {
    case control_noiseLevel:
      midiCCOut(CCnoiseLevel, val);
      myControlChange(midiChannel, CCnoiseLevel, val);
      break;
    case control_pitchLfoRate:
      midiCCOut(CCoscLfoRate, val);
      myControlChange(midiChannel, CCoscLfoRate, val);
      break;
    case control_pitchLfoWaveform:
      midiCCOut(CCoscLfoWaveform, val);
      myControlChange(midiChannel, CCoscLfoWaveform, val);
      break;
    case control_pitchLfoAmount:
      midiCCOut(CCosclfoamt, val);
      myControlChange(midiChannel, CCosclfoamt, val);
      break;
    case control_detune:
      midiCCOut(CCdetune, val);
      myControlChange(midiChannel, CCdetune, val);
      break;
    case control_oscMix:
      midiCCOut(CCoscLevelA, val);
      midiCCOut(CCoscLevelB, val);
      myControlChange(midiChannel, CCoscLevelA, OSCMIXA[val]);
      myControlChange(midiChannel, CCoscLevelB, OSCMIXB[val]);
      break;
    case control_filterAttack:
      midiCCOut(CCfilterattack, val);
      myControlChange(midiChannel, CCfilterattack, val);
      break;
    case control_filterDecay:
      midiCCOut(CCfilterdecay, val);
      myControlChange(midiChannel, CCfilterdecay, val);
      break;
    case control_pwmAmountA:
      midiCCOut(CCpwA, val);
      myControlChange(midiChannel, CCpwA, val);
      break;
    case control_waveformA:
      midiCCOut(CCoscwaveformA, val);
      myControlChange(midiChannel, CCoscwaveformA, val);
      break;
    case control_pitchA:
      midiCCOut(CCpitchA, val);
      myControlChange(midiChannel, CCpitchA, val);
      break;
    case control_pwmAmountB:
      midiCCOut(CCpwB, val);
      myControlChange(midiChannel, CCpwB, val);
      break;
    case control_waveformB:
      midiCCOut(CCoscwaveformB, val);
      myControlChange(midiChannel, CCoscwaveformB, val);
      break;
    case control_pitchB:
      midiCCOut(CCpitchB, val);
      myControlChange(midiChannel, CCpitchB, val);
      break;
    case control_pwmRate:
      midiCCOut(CCpwmRate, val);
      myControlChange(midiChannel, CCpwmRate, val);
      break;
    case control_pitchEnv:
      midiCCOut(CCpitchenv, val);
      myControlChange(midiChannel, CCpitchenv, val);
      break;
    case control_attack:
      midiCCOut(CCampattack, val);
      myControlChange(midiChannel, CCampattack, val);
      break;
    case control_decay:
      midiCCOut(CCampdecay, val);
      myControlChange(midiChannel, CCampdecay, val);
      break;
    case control_sustain:
      midiCCOut(CCampsustain, val);
      myControlChange(midiChannel, CCampsustain, val);
      break;
    case control_release:
      midiCCOut(CCamprelease, val);
      myControlChange(midiChannel, CCamprelease, val);
      break;
    case control_filterLFOAmount:
      midiCCOut(CCfilterlfoamt, val);
      myControlChange(midiChannel, CCfilterlfoamt, val);
      break;
    case control_FXMix:
      midiCCOut(CCfxmix, val);
      myControlChange(midiChannel, CCfxmix, val);
      break;
    case control_FXAmount:
      midiCCOut(CCfxamt, val);
      myControlChange(midiChannel, CCfxamt, val);
      break;
    case control_glide:
      midiCCOut(CCglide, val);
      myControlChange(midiChannel, CCglide, val);
      break;
    case control_filterEnv:
      midiCCOut(CCfilterenv, val);
      myControlChange(midiChannel, CCfilterenv, val);
      break;
    case control_filterRelease:
      midiCCOut(CCfilterrelease, val);
      myControlChange(midiChannel, CCfilterrelease, val);
      break;
    case control_filterSustain:
      midiCCOut(CCfiltersustain, val);
      myControlChange(midiChannel, CCfiltersustain, val);
      break;
    case control_filterType:
      midiCCOut(CCfiltermixer, val);
      myControlChange(midiChannel, CCfiltermixer, val);
      break;
    case control_resonance:
      midiCCOut(CCfilterres, val);
      myControlChange(midiChannel, CCfilterres, val);
      break;
    case control_cutoff:
      midiCCOut(CCfilterfreq, val);
      myControlChange(midiChannel, CCfilterfreq, val);
      break;
    case control_filterLFORate:
      midiCCOut(CCfilterlforate, val);
      myControlChange(midiChannel, CCfilterlforate, val);
      break;
    case control_filterLFOWaveform:
      midiCCOut(CCfilterlfowaveform, val);
      myControlChange(midiChannel, CCfilterlfowaveform, val);
      break;
    case control_volume:
      myControlChange(midiChannel, CCvolume, val);
      break;
    case control_unison:
      midiCCOut(CCunison, groupvec[activeGroupIndex]->params().unisonMode == 2 ? 0 : groupvec[activeGroupIndex]->params().unisonMode + 1);
      myControlChange(midiChannel, CCunison, groupvec[activeGroupIndex]->params().unisonMode == 2 ? 0 : groupvec[activeGroupIndex]->params().unisonMode + 1);
      break;
    case control_oscFX:
      midiCCOut(CCoscfx, groupvec[activeGroupIndex]->getOscFX() == 2 ? 0 : groupvec[activeGroupIndex]->getOscFX() + 1);
      myControlChange(midiChannel, CCoscfx, groupvec[activeGroupIndex]->getOscFX() == 2 ? 0 : groupvec[activeGroupIndex]->getOscFX() + 1);
      break;
    case control_filterLFORetrig:
      bool val2 = !groupvec[activeGroupIndex]->getFilterLfoRetrig();
      midiCCOut(CCfilterlforetrig, val2);
      myControlChange(midiChannel, CCfilterlforetrig, val2);
      break;
    case control_filterLFOMidiClkSync:
      bool value3 = !groupvec[activeGroupIndex]->getFilterLfoMidiClockSync();
      midiCCOut(CCfilterLFOMidiClkSync, value3);
      myControlChange(midiChannel, CCfilterLFOMidiClkSync, value3);
      break;
  }
*/

uint8_t update(uint8_t currentParameter, int value, int8_t delta, uint8_t range)
{
  // if(range<25){
  //   uint8_t val = map(value, 0, 127, 0, 24);
  // }
  if ((currentParameter + delta > -1) && (currentParameter + delta < range + 1))
  {
    currentParameter += delta;
  }
  return currentParameter;
}

void encoderCallback(unsigned enc_idx, int value, int delta)
{
  Serial.printf("enc[%u]: v=%d, d=%d\n", enc_idx, value, delta);
  switch (enc_idx)
  {
  case ENC_TR:
    currentPatch.FilterFreq = update(currentPatch.FilterFreq, value, delta, 255);
    myControlChange(midiChannel, filterfreq256, currentPatch.FilterFreq); // 8 bit
    midiCCOut(CCfilterfreq, currentPatch.FilterFreq >> 1);                // Convert 8 bit to 7 bit
    break;
  case ENC_BR:
    currentPatch.FilterRes = update(currentPatch.FilterRes, value, delta, 127);
    midiCCOut(CCfilterres, currentPatch.FilterRes);
    myControlChange(midiChannel, CCfilterres, currentPatch.FilterRes);
    break;
  case ENC_TL:
    currentPatch.Unison = update(currentPatch.Unison, value, delta, 2);
    midiCCOut(CCunison, currentPatch.Unison);
    myControlChange(midiChannel, CCunison, currentPatch.Unison);
    break;
  case ENC_BL:
    break;
  }
}

void encoderButtonCallback(unsigned enc_idx, int state)
{
  switch (enc_idx)
  {
  case 4:
    // Encoder TR
    Serial.println("Encoder TR");
    break;
  case 5:
    // Encoder BR
    Serial.println("Encoder BR");
    break;
  case 6:
    // Encoder TL
    Serial.println("Encoder TL");
    break;
  case 7:
    // Encoder BL
    Serial.println("Encoder BL");
    break;
  }
}

void showSettingsPage()
{
  showSettingsPage(settings::current_setting(), settings::current_setting_value(), state);
}
/*
  void checkSwitches()
  {
  unisonSwitch.update();
  if (unisonSwitch.numClicks() == 1)
  {
    //Cycle through each option
    midiCCOut(CCunison, groupvec[activeGroupIndex]->params().unisonMode == 2 ? 0 : groupvec[activeGroupIndex]->params().unisonMode + 1);
    myControlChange(midiChannel, CCunison, groupvec[activeGroupIndex]->params().unisonMode == 2 ? 0 : groupvec[activeGroupIndex]->params().unisonMode + 1);
  }

  oscFXSwitch.update();
  if (oscFXSwitch.numClicks() == 1)
  {
    //Cycle through each option
    midiCCOut(CCoscfx, groupvec[activeGroupIndex]->getOscFX() == 2 ? 0 : groupvec[activeGroupIndex]->getOscFX() + 1);
    myControlChange(midiChannel, CCoscfx, groupvec[activeGroupIndex]->getOscFX() == 2 ? 0 : groupvec[activeGroupIndex]->getOscFX() + 1);
  }

  filterLFORetrigSwitch.update();
  if (filterLFORetrigSwitch.numClicks() == 1)
  {
    bool value = !groupvec[activeGroupIndex]->getFilterLfoRetrig();
    midiCCOut(CCfilterlforetrig, value);
    myControlChange(midiChannel, CCfilterlforetrig, value);
  }

  tempoSwitch.update();
  if (tempoSwitch.numClicks() == 1)
  {
    bool value = !groupvec[activeGroupIndex]->getFilterLfoMidiClockSync();
    midiCCOut(CCfilterLFOMidiClkSync, value);
    myControlChange(midiChannel, CCfilterLFOMidiClkSync, value);
  }

  saveButton.update();
  if (saveButton.held())
  {
    switch (state)
    {
    case PARAMETER:
    case PATCH:
      state = DELETE;
      break;
    }
  }
  else if (saveButton.numClicks() == 1)
  {
    switch (state)
    {
    case PARAMETER:
      if (patches.size() < PATCHES_LIMIT)
      {
        resetPatchesOrdering(); //Reset order of patches from first patch
        patches.push({patches.size() + 1, INITPATCHNAME});
        state = SAVE;
      }
      break;
    case SAVE:
      //Save as new patch with INITIALPATCH name or overwrite existing keeping name - bypassing patch renaming
      patchName = patches.last().patchName;
      state = PATCH;
      savePatch(String(patches.last().patchNo).c_str(), getCurrentPatchData());
      showPatchPage(patches.last().patchNo, patches.last().patchName);
      patchNo = patches.last().patchNo;
      loadPatches(); //Get rid of pushed patch if it wasn't saved
      setPatchesOrdering(patchNo);
      renamedPatch = "";
      state = PARAMETER;
      break;
    case PATCHNAMING:
      if (renamedPatch.length() > 0)
        patchName = renamedPatch; //Prevent empty strings
      state = PATCH;
      savePatch(String(patches.last().patchNo).c_str(), getCurrentPatchData());
      showPatchPage(patches.last().patchNo, patchName);
      patchNo = patches.last().patchNo;
      loadPatches(); //Get rid of pushed patch if it wasn't saved
      setPatchesOrdering(patchNo);
      renamedPatch = "";
      state = PARAMETER;
      break;
    }
  }

  settingsButton.update();
  if (settingsButton.held())
  {
    //If recall held, set current patch to match current hardware state
    //Reinitialise all hardware values to force them to be re-read if different
    state = REINITIALISE;
    reinitialiseToPanel();
  }
  else if (settingsButton.numClicks() == 1)
  {
    switch (state)
    {
    case PARAMETER:
      state = SETTINGS;
      showSettingsPage();
      break;
    case SETTINGS:
      showSettingsPage();
    case SETTINGSVALUE:
      settings::save_current_value();
      state = SETTINGS;
      showSettingsPage();
      break;
    }
  }

  backButton.update();
  if (backButton.held())
  {
    //If Back button held, Panic - all notes off
    groupvec[activeGroupIndex]->allNotesOff();
    groupvec[activeGroupIndex]->closeEnvelopes();
  }
  else if (backButton.numClicks() == 1)
  {
    switch (state)
    {
    case RECALL:
      setPatchesOrdering(patchNo);
      state = PARAMETER;
      break;
    case SAVE:
      renamedPatch = "";
      state = PARAMETER;
      loadPatches(); //Remove patch that was to be saved
      setPatchesOrdering(patchNo);
      break;
    case PATCHNAMING:
      charIndex = 0;
      renamedPatch = "";
      state = SAVE;
      break;
    case DELETE:
      setPatchesOrdering(patchNo);
      state = PARAMETER;
      break;
    case SETTINGS:
      state = PARAMETER;
      break;
    case SETTINGSVALUE:
      state = SETTINGS;
      showSettingsPage();
      break;
    }
  }

  //Encoder switch
  recallButton.update();
  if (recallButton.held())
  {
    //If Recall button held, return to current patch setting
    //which clears any changes made
    state = PATCH;
    //Recall the current patch
    patchNo = patches.first().patchNo;
    recallPatch(patchNo);
    state = PARAMETER;
  }
  else if (recallButton.numClicks() == 1)
  {
    switch (state)
    {
    case PARAMETER:
      state = RECALL; //show patch list
      break;
    case RECALL:
      state = PATCH;
      //Recall the current patch
      patchNo = patches.first().patchNo;
      recallPatch(patchNo);
      state = PARAMETER;
      break;
    case SAVE:
      showRenamingPage(patches.last().patchName);
      patchName = patches.last().patchName;
      state = PATCHNAMING;
      break;
    case PATCHNAMING:
      if (renamedPatch.length() < 12) //actually 12 chars
      {
        renamedPatch.concat(String(currentCharacter));
        charIndex = 0;
        currentCharacter = CHARACTERS[charIndex];
        showRenamingPage(renamedPatch);
      }
      break;
    case DELETE:
      //Don't delete final patch
      if (patches.size() > 1)
      {
        state = DELETEMSG;
        patchNo = patches.first().patchNo;    //PatchNo to delete from SD card
        patches.shift();                      //Remove patch from circular buffer
        deletePatch(String(patchNo).c_str()); //Delete from SD card
        loadPatches();                        //Repopulate circular buffer to start from lowest Patch No
        renumberPatchesOnSD();
        loadPatches();                     //Repopulate circular buffer again after delete
        patchNo = patches.first().patchNo; //Go back to 1
        recallPatch(patchNo);              //Load first patch
      }
      state = PARAMETER;
      break;
    case SETTINGS:
      state = SETTINGSVALUE;
      showSettingsPage();
      break;
    case SETTINGSVALUE:
      settings::save_current_value();
      state = SETTINGS;
      showSettingsPage();
      break;
    }
  }
  }



  void checkEncoder()
  {
  //Encoder works with relative inc and dec values
  //Detent encoder goes up in 4 steps, hence +/-3
  long encRead = encoder.read();
  if ((encCW && encRead > encPrevious + 3) || (!encCW && encRead < encPrevious - 3))
  {
    switch (state)
    {
    case PARAMETER:
      state = PATCH;
      patches.push(patches.shift());
      patchNo = patches.first().patchNo;
      recallPatch(patchNo);
      state = PARAMETER;
      // Make sure the current setting value is refreshed.
      settings::increment_setting();
      settings::decrement_setting();
      break;
    case RECALL:
      patches.push(patches.shift());
      break;
    case SAVE:
      patches.push(patches.shift());
      break;
    case PATCHNAMING:
      if (charIndex == TOTALCHARS)
        charIndex = 0; //Wrap around
      currentCharacter = CHARACTERS[charIndex++];
      showRenamingPage(renamedPatch + currentCharacter);
      break;
    case DELETE:
      patches.push(patches.shift());
      break;
    case SETTINGS:
      settings::increment_setting();
      showSettingsPage();
      break;
    case SETTINGSVALUE:
      settings::increment_setting_value();
      showSettingsPage();
      break;
    }
    encPrevious = encRead;
  }
  else if ((encCW && encRead < encPrevious - 3) || (!encCW && encRead > encPrevious + 3))
  {
    switch (state)
    {
    case PARAMETER:
      state = PATCH;
      patches.unshift(patches.pop());
      patchNo = patches.first().patchNo;
      recallPatch(patchNo);
      state = PARAMETER;
      // Make sure the current setting value is refreshed.
      settings::increment_setting();
      settings::decrement_setting();
      break;
    case RECALL:
      patches.unshift(patches.pop());
      break;
    case SAVE:
      patches.unshift(patches.pop());
      break;
    case PATCHNAMING:
      if (charIndex == -1)
        charIndex = TOTALCHARS - 1;
      currentCharacter = CHARACTERS[charIndex--];
      showRenamingPage(renamedPatch + currentCharacter);
      break;
    case DELETE:
      patches.unshift(patches.pop());
      break;
    case SETTINGS:
      settings::decrement_setting();
      showSettingsPage();
      break;
    case SETTINGSVALUE:
      settings::decrement_setting_value();
      showSettingsPage();
      break;
    }
    encPrevious = encRead;
  }
  }
*/

void buttonCallback(unsigned button_idx, int state)
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
  case VOL_DOWN:
    if (state == HIGH)
      Serial.println("Vol Down");
    currentVolume = clampToRange<int>(currentVolume, -1, 0, 127);
    myControlChange(midiChannel, CCvolume, currentVolume);
    break;
  case VOL_UP:
    if (state == HIGH)
      Serial.println("Vol Up");
    currentVolume = clampToRange<int>(currentVolume, 1, 0, 127);
    myControlChange(midiChannel, CCvolume, currentVolume);
    break;
  case BUTTON_8:
    if (state == HIGH)
      singleLED(RED, 8);
    break;
  case BUTTON_7:
    if (state == HIGH)
      singleLED(RED, 7);

    // switch (state)
    // {
    // case PARAMETER:
    //   if (patches.size() < PATCHES_LIMIT)
    //   {
    //     resetPatchesOrdering(); // Reset order of patches from first patch
    //     patches.push({patches.size() + 1, INITPATCHNAME});
    //     state = SAVE;
    //   }
    //   break;
    // case SAVE:
    //   // Save as new patch with INITIALPATCH name or overwrite existing keeping name - bypassing patch renaming
    //   patchName = patches.last().patchName;
    //   state = PATCH;
    //   savePatch(String(patches.last().patchNo).c_str(), getCurrentPatchData());
    //   showPatchPage(patches.last().patchNo, patches.last().patchName);
    //   patchNo = patches.last().patchNo;
    //   loadPatches(); // Get rid of pushed patch if it wasn't saved
    //   // setPatchesOrdering(patchNo);
    //   renamedPatch = "";
    //   state = PARAMETER;
    //   break;
    // case PATCHNAMING:
    //   if (renamedPatch.length() > 0)
    //     patchName = renamedPatch; // Prevent empty strings
    //   state = PATCH;
    //   savePatch(String(patches.last().patchNo).c_str(), getCurrentPatchData());
    //   showPatchPage(patches.last().patchNo, patchName);
    //   patchNo = patches.last().patchNo;
    //   loadPatches(); // Get rid of pushed patch if it wasn't saved
    //   setPatchesOrdering(patchNo);
    //   renamedPatch = "";
    //   state = PARAMETER;
    //   break;
    // }
    break;
  case BUTTON_5:
    if (state == HIGH)
      singleLED(RED, 5);
    break;
  case BUTTON_4:
    if (state == HIGH)
      singleLED(RED, 4);
    break;
  case BUTTON_6:
    if (state == HIGH)
      singleLED(RED, 6);
    break;
  case BUTTON_2:
    if (state == HIGH)
      singleLED(RED, 2);
    break;
  case BUTTON_1:
    if (state == HIGH)
      singleLED(RED, 1);
    break;
  case BUTTON_3:
    if (state == HIGH)
      singleLED(RED, 3);
    break;
  }
}

// None-285mA R-45mA  G-25mA  RG-70mA
void lightLEDs()
{
  // lightRGLEDs(0, 0);
}

void midiCCOut(byte cc, byte value)
{
  if (midiOutCh > 0)
  {
    usbMIDI.sendControlChange(cc, value, midiOutCh);
    midi1.sendControlChange(cc, value, midiOutCh);
    if (MIDIThru == midi::Thru::Off)
      MIDI.sendControlChange(cc, value, midiOutCh); // MIDI DIN is set to Out
  }
}

void CPUMonitor()
{
  Serial.print(F(" CPU:"));
  Serial.print(AudioProcessorUsage());
  Serial.print(F(" ("));
  Serial.print(AudioProcessorUsageMax());
  Serial.print(F(")"));
  Serial.print(F("  MEM:"));
  Serial.println(AudioMemoryUsageMax());
  delayMicroseconds(500);
}

void loop()
{
  // USB HOST MIDI Class Compliant
  myusb.Task();
  midi1.read(midiChannel);
  // USB Client MIDI
  usbMIDI.read(midiChannel);
  // MIDI 5 Pin DIN
  MIDI.read(midiChannel);
  encoders.read();
  buttons.read();
  lightLEDs();
  // CPUMonitor();
}
