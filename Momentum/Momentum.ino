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
#include "MidiCC.h"

#include "SettingsService.h"
#include "AudioPatching.h"
#include "Constants.h"
#include "Parameters.h"
#include "PatchMgr.h"
#include "SequenceMgr.h"
#include "PerformanceMgr.h"
#include "HWControls.h"
#include "StringMapping.h"
#include "EncoderMapping.h"
#include "EepromMgr.h"
#include "Detune.h"
#include "utils.h"
#include "Voice.h"
#include "VoiceGroup.h"
//#include "Sequencer.h"

State state = State::PARAMETER;

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
    checkSDCardStructure();
    loadBankNames();
    loadPatchNames(currentBankIndex);
    recallPatch(currentBankIndex, patches.front().patchUID); // Load first patch from SD card
  }
  else
  {
    setCurrentPatchData(); // Initialise to default
    Serial.println(F("SD card is not connected or unusable"));
    showPatchPage(F("SD Card"), F("Not connected or usable"));
  }

  // Read MIDI Channel from EEPROM
  midiChannel = getMIDIChannel();
  Serial.println(F("MIDI In Channel:") + String(midiChannel) + F(" (0 is Omni On)"));

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

  assignStrings();

  setEncodersState(state);
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
    return F("Variable Pulse");
  case WAVEFORM_TRIANGLE_VARIABLE:
    return F("Variable Triangle");
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
    showCurrentParameterPage("Dynamic Unison", "On");
  }
  else
  {
    showCurrentParameterPage("Chord Unison", "On");
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
  showCurrentParameterPage("1 Waveform", getWaveformStr(waveform));
}

FLASHMEM void updateWaveformB(uint32_t waveform)
{
  groupvec[activeGroupIndex]->setWaveformB(waveform);
  showCurrentParameterPage("2 Waveform", getWaveformStr(waveform));
}

FLASHMEM void updatePitchA(int pitch)
{
  groupvec[activeGroupIndex]->params().oscPitchA = pitch;
  groupvec[activeGroupIndex]->updateVoices();
  showCurrentParameterPage("1 Semitones", (pitch > 0 ? "+" : "") + String(pitch));
}

FLASHMEM void updatePitchB(int pitch)
{
  groupvec[activeGroupIndex]->params().oscPitchB = pitch;
  groupvec[activeGroupIndex]->updateVoices();
  showCurrentParameterPage("2 Semitones", (pitch > 0 ? "+" : "") + String(pitch));
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
    showCurrentParameterPage("Pulse Width Mode", "On");
  }
  else if (value == PWMRATE_SOURCE_FILTER_ENV)
  {
    // Set to Filter Env Mod source
    showCurrentParameterPage("PWM Source", "Filter Envelope");
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
  showCurrentParameterPage("PWM Amount", String(value) + " : " + String(value));
}

FLASHMEM void updatePWA(float valuePwA, float valuePwmAmtA)
{
  groupvec[activeGroupIndex]->setPWA(valuePwA, valuePwmAmtA);

  if (groupvec[activeGroupIndex]->getPwmRate() == PWMRATE_PW_MODE)
  {
    if (groupvec[activeGroupIndex]->getWaveformA() == WAVEFORM_TRIANGLE_VARIABLE)
    {
      showCurrentParameterPage("1 Variable Triangle", groupvec[activeGroupIndex]->getPwA(), VAR_TRI);
    }
    else
    {
      showCurrentParameterPage("1 Pulse Width", groupvec[activeGroupIndex]->getPwA(), PULSE);
    }
  }
  else
  {
    if (groupvec[activeGroupIndex]->getPwmSource() == PWMSOURCELFO)
    {
      // PW alters PWM LFO amount for waveform A
      showCurrentParameterPage("1 PWM Amount", "LFO " + String(groupvec[activeGroupIndex]->getPwmAmtA()));
    }
    else
    {
      // PW alters PWM Filter Env amount for waveform A
      showCurrentParameterPage("1 PWM Amount", "Filter Envelope " + String(groupvec[activeGroupIndex]->getPwmAmtA()));
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
      showCurrentParameterPage("2 Variable Triangle", groupvec[activeGroupIndex]->getPwB(), VAR_TRI);
    }
    else
    {
      showCurrentParameterPage("2 Pulse Width", groupvec[activeGroupIndex]->getPwB(), PULSE);
    }
  }
  else
  {
    if (groupvec[activeGroupIndex]->getPwmSource() == PWMSOURCELFO)
    {
      // PW alters PWM LFO amount for waveform B
      showCurrentParameterPage("2 PWM Amount", "LFO " + String(groupvec[activeGroupIndex]->getPwmAmtB()));
    }
    else
    {
      // PW alters PWM Filter Env amount for waveform B
      showCurrentParameterPage("2 PWM Amount", "Filter Envelope " + String(groupvec[activeGroupIndex]->getPwmAmtB()));
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
      showCurrentParameterPage("X-Mod Osc 1", "by Osc 2: " + String(1 - groupvec[activeGroupIndex]->getOscLevelB()));
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
      showCurrentParameterPage("X-Mod Osc 2", "by Osc 1: " + String(1 - groupvec[activeGroupIndex]->getOscLevelA()));
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
  showCurrentParameterPage("Cutoff Frequency", String(int(value)) + " Hz");
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
      filterStr = "Low " + String(100 - int(100 * value)) + " - " + String(int(100 * value)) + " High";
    }
  }

  showCurrentParameterPage("Filter Type", filterStr);
}

FLASHMEM void updateFilterEnv(float value)
{
  groupvec[activeGroupIndex]->setFilterEnvelope(value);
  showCurrentParameterPage("Filter Envelope", String(value));
}

FLASHMEM void updatePitchEnv(float value)
{
  groupvec[activeGroupIndex]->setPitchEnvelope(value);
  showCurrentParameterPage("Pitch Env Amount", String(value));
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
  showCurrentParameterPage("Pitch LFO Sync", value ? "On" : "Off");
}

FLASHMEM void updateFilterLfoRate(float value, String timeDivStr)
{
  groupvec[activeGroupIndex]->setFilterLfoRate(value);

  if (timeDivStr.length() > 0)
  {
    showCurrentParameterPage("LFO Time Division", timeDivStr);
  }
  else
  {
    showCurrentParameterPage("Filter LFO Rate", String(value) + " Hz");
  }
}

FLASHMEM void updateFilterLfoAmt(float value)
{
  groupvec[activeGroupIndex]->setFilterLfoAmt(value);
  showCurrentParameterPage("Filter LFO Amount", String(value));
}

FLASHMEM void updateFilterLFOWaveform(uint32_t waveform)
{
  groupvec[activeGroupIndex]->setFilterLfoWaveform(waveform);
  showCurrentParameterPage("Filter LFO", getWaveformStr(waveform));
}

FLASHMEM void updatePitchLFORetrig(bool value)
{
  groupvec[activeGroupIndex]->setPitchLfoRetrig(value);
  showCurrentParameterPage("Pitch LFO Retrigger", value ? "On" : "Off");
}

FLASHMEM void updateFilterLFORetrig(bool value)
{
  groupvec[activeGroupIndex]->setFilterLfoRetrig(value);
  showCurrentParameterPage("Filter LFO Retrigger", groupvec[activeGroupIndex]->getFilterLfoRetrig() ? "On" : "Off");
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
  currentPatch.Attack = value;
  groupvec[activeGroupIndex]->setAmpAttack(currentPatch.Attack);
  showCurrentParameterPage("Attack", milliToString(currentPatch.Attack), AMP_ENV);
}

FLASHMEM void updateDecay(float value)
{
  currentPatch.Decay = value;
  groupvec[activeGroupIndex]->setAmpDecay(currentPatch.Decay);
  showCurrentParameterPage("Decay", milliToString(currentPatch.Decay), AMP_ENV);
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
    showCurrentParameterPage("Osc Effect", "X Mod");
  }
  else if (value == 1)
  {
    showCurrentParameterPage("Osc Effect", "XOR Mod");
  }
  else
  {
    showCurrentParameterPage("Osc Effect", "Off");
  }
}

FLASHMEM void updateEffectAmt(float value)
{
  groupvec[activeGroupIndex]->setEffectAmount(value);
  showCurrentParameterPage("Effect Amount", String(value) + " Hz");
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
    currentVolume = value;
    updateVolume(LINEAR[currentVolume]);
    break;
  case CCunison:
    currentPatch.Unison = value;
    updateUnison(inRangeOrDefault<int>(currentPatch.Unison, 2, 0, 2));
    break;

  case CCglide:
    currentPatch.Glide = value;
    updateGlide(POWER[value]);
    break;

  case CCpitchenv:
    currentPatch.PitchEnv = value;
    updatePitchEnv(LINEARCENTREZERO[value] * OSCMODMIXERMAX);
    break;

  case CCoscwaveformA:
    currentPatch.WaveformA = value;
    updateWaveformA(getWaveformA(value));
    break;

  case CCoscwaveformB:
    currentPatch.WaveformB = value;
    updateWaveformB(getWaveformB(value));
    break;

  case CCpitchA:
    currentPatch.PitchA = value;
    updatePitchA(PITCH[value]);
    break;

  case CCpitchB:
    currentPatch.PitchB = value;
    updatePitchB(PITCH[value]);
    break;

  case CCdetune:
    currentPatch.Detune = value;
    updateDetune(1.0f - (MAXDETUNE * POWER[value]), value);
    break;

  case CCpwmSource:
    currentPatch.PWMSource = value;
    updatePWMSource(value > 0 ? PWMSOURCEFENV : PWMSOURCELFO);
    break;

  case CCpwmRate:
    currentPatch.PWMRate = value;
    // Uses combination of PWMRate, PWa and PWb
    updatePWMRate(PWMRATE[value]);
    break;

  case CCpwmAmt:
    currentPatch.PWMA_Amount = value;
    currentPatch.PWMB_Amount = value;
    // Total PWM amount for both oscillators
    updatePWMAmount(LINEAR[value]);
    break;

  case CCpwmAmtA:
    currentPatch.PWMA_Amount = value;
    // Need to set both PW amount and PWM amount due to a choice of PWM source switching between them
    updatePWA(groupvec[activeGroupIndex]->getPwA(), LINEAR[value]);
    break;

  case CCpwmAmtB:
    currentPatch.PWMB_Amount = value;
    // Need to set both PW amount and PWM amount due to a choice of PWM source switching between them
    updatePWB(groupvec[activeGroupIndex]->getPwB(), LINEAR[value]);
    break;

  case CCpwA:
    currentPatch.PWA_Amount = value;
    updatePWA(LINEARCENTREZERO[value], LINEAR[value]);
    break;

  case CCpwB:
    currentPatch.PWB_Amount = value;
    updatePWB(LINEARCENTREZERO[value], LINEAR[value]);
    break;

  case CCoscLevelA:
    currentPatch.OscLevelA = value;
    updateOscLevelA(LINEAR[value]);
    break;

  case CCoscLevelB:
    currentPatch.OscLevelB = value;
    updateOscLevelB(LINEAR[value]);
    break;

  case CCnoiseLevel:
    currentPatch.NoiseLevel = value;
    updateNoiseLevel(LINEARCENTREZERO[value]);
    break;

  case CCbankselectLSB:
    if (value < BANKS_LIMIT)
    {
      Serial.println(value);
      currentBankIndex = value;
      loadPatchNames(currentBankIndex);
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
    Serial.println(currentPatch.FilterFreq);
    updateFilterFreq(FILTERFREQS256[currentPatch.FilterFreq]);
    break;

  case filterfreq256:
    // 8 bit from panel control for smoothness
    currentPatch.FilterFreq = value;
    updateFilterFreq(FILTERFREQS256[currentPatch.FilterFreq]);
    break;

  case CCfilterres:
    currentPatch.FilterRes = value;
    updateFilterRes((14.29f * POWER[value]) + 0.71f);
    break;

  case CCfiltermixer:
    currentPatch.FilterMixer = value;
    updateFilterMixer(LINEAR_FILTERMIXER[value]);
    break;

  case CCfilterenv:
    currentPatch.FilterEnv = value;
    updateFilterEnv(LINEARCENTREZERO[value] * FILTERMODMIXERMAX);
    break;

  case CCkeytracking:
    currentPatch.KeyTracking = value;
    updateKeyTracking(KEYTRACKINGAMT[value]);
    break;

  case CCmodwheel:
    // Variable LFO amount from mod wheel - Settings Option
    updateModWheel(POWER[value] * modWheelDepth);
    break;

  case CCosclfoamt:
    currentPatch.PitchLFOAmt = value;
    updatePitchLFOAmt(POWER[value]);
    break;

  case CCoscLfoRate:
  {
    currentPatch.PitchLFORate = value;
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
    currentPatch.PitchLFOWaveform = value;
    updatePitchLFOWaveform(getLFOWaveform(value));
    break;

  case CCosclforetrig:
    currentPatch.PitchLFORetrig = value;
    updatePitchLFORetrig(value > 0);
    break;

  case CCfilterLFOMidiClkSync:
    currentPatch.FilterLFOMidiClkSync = value;
    updateFilterLFOMidiClkSync(value > 0);
    break;

  case CCfilterlforate:
  {
    currentPatch.FilterLFORate = value;
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
    currentPatch.FilterLfoAmt = value;
    updateFilterLfoAmt(LINEAR[value] * FILTERMODMIXERMAX);
    break;

  case CCfilterlfowaveform:
    currentPatch.FilterLFOWaveform = value;
    updateFilterLFOWaveform(getLFOWaveform(value));
    break;

  case CCfilterlforetrig:
    currentPatch.FilterLFORetrig = value;
    updateFilterLFORetrig(value > 0);
    break;

  case CCoscLFOMidiClkSync:
    currentPatch.PitchLFOMidiClkSync = value;
    updatePitchLFOMidiClkSync(value > 0);
    break;

  case CCfilterattack:
    currentPatch.FilterAttack = value;
    updateFilterAttack(ENVTIMES[value]);
    break;

  case CCfilterdecay:
    currentPatch.FilterDecay = value;
    updateFilterDecay(ENVTIMES[value]);
    break;

  case CCfiltersustain:
    currentPatch.FilterSustain = value;
    updateFilterSustain(LINEAR[value]);
    break;

  case CCfilterrelease:
    currentPatch.FilterRelease = value;
    updateFilterRelease(ENVTIMES[value]);
    break;

  case CCampattack:
    currentPatch.Attack = value;
    updateAttack(ENVTIMES[value]);
    break;

  case CCampdecay:
    currentPatch.Decay = value;
    updateDecay(ENVTIMES[value]);
    break;

  case CCampsustain:
    currentPatch.Sustain = value;
    updateSustain(LINEAR[value]);
    break;

  case CCamprelease:
    currentPatch.Release = value;
    updateRelease(ENVTIMES[value]);
    break;

  case CCoscfx:
    currentPatch.OscFX = value;
    updateOscFX(inRangeOrDefault<int>(value, 2, 0, 2));
    break;

  case CCfxamt:
    currentPatch.EffectAmt = value;
    updateEffectAmt(ENSEMBLE_LFO[value]);
    break;

  case CCfxmix:
    currentPatch.EffectMix = value;
    updateEffectMix(LINEAR[value]);
    break;

  case CCallnotesoff:
    groupvec[activeGroupIndex]->allNotesOff();
    break;
  }
}

FLASHMEM void myProgramChange(byte channel, byte program)
{
  if (program < patches.size())
  {
    // state = PATCH;
    currentPatchIndex = program;
    recallPatch(currentBankIndex, patches[currentPatchIndex].patchUID);
    Serial.print(F("MIDI Pgm Change:"));
    Serial.println(patches[currentPatchIndex].patchUID);
    // state = PARAMETER;
  }
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

FLASHMEM void recallPatch(uint8_t bank, long patchUID)
{
  if (patchUID == 0)
    return;
  groupvec[activeGroupIndex]->allNotesOff();
  groupvec[activeGroupIndex]->closeEnvelopes();
  loadPatch(bank, patchUID);
  setCurrentPatchData();
}

FLASHMEM void setCurrentPatchData()
{
  updatePatch(currentPatch.PatchName, currentPatchIndex + 1, currentPatch.UID);
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
  myControlChange(midiChannel, CCpwmSource, currentPatch.PWMSource);
  myControlChange(midiChannel, CCpwA, currentPatch.PWA_Amount);
  myControlChange(midiChannel, CCpwmAmtA, currentPatch.PWMA_Amount);
  myControlChange(midiChannel, CCpwB, currentPatch.PWB_Amount);
  myControlChange(midiChannel, CCpwmAmtB, currentPatch.PWMB_Amount);
  myControlChange(midiChannel, CCpwmRate, currentPatch.PWMRate);
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
  velocitySens = currentPatch.VelocitySensitivity;
  groupvec[activeGroupIndex]->params().chordDetune = currentPatch.ChordDetune;
  groupvec[activeGroupIndex]->setMonophonic(currentPatch.MonophonicMode);
  updateDisplay = true;

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

FLASHMEM uint8_t update(uint8_t currentParameter, int8_t delta, uint8_t range)
{
  if (range < 25)
  {
    // TODO
  }
  if ((currentParameter + delta > -1) && (currentParameter + delta < range + 1))
  {
    currentParameter += delta;
  }
  return currentParameter;
}

FLASHMEM void encoderCallback(unsigned enc_idx, int value, int delta)
{
  Serial.printf("enc[%u]: v=%d, d=%d\n", enc_idx, value, delta);
  // Subtract 4 from encoder index due to numbering on shift registers
  enc_idx -= 4;
  if (encMap[enc_idx].active)
  {
    encMap[enc_idx].Value = update(encMap[enc_idx].Value, delta, encMap[enc_idx].Range);
    switch (encMap[enc_idx].Parameter)
    {
    case patchselect:
      recallPatch(currentBankIndex, patches[delta > 0 ? incCurrentPatchIndex() : decCurrentPatchIndex()].patchUID);
      break;
    case CCbankselectLSB:
      loadPatchNames(delta > 0 ? incTempBankIndex() : decTempBankIndex());
      if (state == State::RECALL)
      {
        currentBankIndex = tempBankIndex;
        currentPatchIndex = 0;
        recallPatch(currentBankIndex, patches[0].patchUID);
      }
      else
      {
        while (patches[0].patchUID == 0)
        {
          loadPatchNames(delta > 0 ? incTempBankIndex() : decTempBankIndex());
        }
        currentBankIndex = tempBankIndex;
        currentPatchIndex = 0;
        recallPatch(currentBankIndex, patches[0].patchUID);
      }
      break;
    default:
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
  if (encMap[enc_idx].Push)
  {
    state = encMap[enc_idx].PushActionState;
  }
}

FLASHMEM void showSettingsPage()
{
  showSettingsPage(settings::current_setting(), settings::current_setting_value(), state);
}
/*
  {
  unisonSwitch
    //Cycle through each option
    midiCCOut(CCunison, groupvec[activeGroupIndex]->params().unisonMode == 2 ? 0 : groupvec[activeGroupIndex]->params().unisonMode + 1);
    myControlChange(midiChannel, CCunison, groupvec[activeGroupIndex]->params().unisonMode == 2 ? 0 : groupvec[activeGroupIndex]->params().unisonMode + 1);

  oscFXSwitch
    //Cycle through each option
    midiCCOut(CCoscfx, groupvec[activeGroupIndex]->getOscFX() == 2 ? 0 : groupvec[activeGroupIndex]->getOscFX() + 1);
    myControlChange(midiChannel, CCoscfx, groupvec[activeGroupIndex]->getOscFX() == 2 ? 0 : groupvec[activeGroupIndex]->getOscFX() + 1);


  filterLFORetrigSwitch
    bool value = !groupvec[activeGroupIndex]->getFilterLfoRetrig();
    midiCCOut(CCfilterlforetrig, value);
    myControlChange(midiChannel, CCfilterlforetrig, value);


  tempoSwitch
    bool value = !groupvec[activeGroupIndex]->getFilterLfoMidiClockSync();
    midiCCOut(CCfilterLFOMidiClkSync, value);
    myControlChange(midiChannel, CCfilterLFOMidiClkSync, value);


  saveButton.update();
  if (saveButton.held())
  {

  }
  else if (saveButton.numClicks() == 1)
  {
    switch (state)
    {
    case PARAMETER:
      if (patches.size() < PATCHES_LIMIT)
      {
        patches.push_back(UID);
        state = SAVE;
      }
      break;
    case SAVE:
      //Save as new patch with INITIALPATCH name or overwrite existing keeping name - bypassing patch renaming
      patchName = patches.back().patchName;
      state = PATCH;
      savePatch(String(patches.back().patchNo).c_str(), getCurrentPatchData());
      showPatchPage(patches.back().patchNo, patches.back().patchName);
      patchNo = patches.back().patchNo;
      loadPatches(); //Get rid of pushed patch if it wasn't saved
      setPatchesOrdering(patchNo);
      renamedPatch = "";
      state = PARAMETER;
      break;
    case PATCHNAMING:
      if (renamedPatch.length() > 0)
        patchName = renamedPatch; //Prevent empty strings
      state = PATCH;
      savePatch(String(patches.back().patchNo).c_str(), getCurrentPatchData());
      showPatchPage(patches.back().patchNo, patchName);
      patchNo = patches.back().patchNo;
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
    patchNo = patches.front().patchNo;
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
      patchNo = patches.front().patchNo;
      recallPatch(patchNo);
      state = PARAMETER;
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
        patchNo = patches.front().patchNo;    //PatchNo to delete from SD card
        patches.shift();                      //Remove patch from circular buffer
        deletePatch(String(patchNo).c_str()); //Delete from SD card
        loadPatches();                        //Repopulate circular buffer to start from lowest Patch No
        renumberPatchesOnSD();
        loadPatches();                     //Repopulate circular buffer again after delete
        patchNo = patches.front().patchNo; //Go back to 1
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
      recallPatch(patches[incCurrentPatchIndex()].patchNo);
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
      recallPatch(patches[decCurrentPatchIndex()].patchNo);
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

      // switch (state)
    // {


    // case PATCHNAMING:
    //   if (renamedPatch.length() > 0)
    //     patchName = renamedPatch; // Prevent empty strings
    //   state = PATCH;
    //   savePatch(String(patches.back().patchNo).c_str(), getCurrentPatchData());
    //   showPatchPage(patches.back().patchNo, patchName);
    //   patchNo = patches.back().patchNo;
    //   loadPatches(); // Get rid of pushed patch if it wasn't saved
    //   setPatchesOrdering(patchNo);
    //   renamedPatch = "";
    //   state = PARAMETER;
    //   break;
    // }
    break;
*/

void buttonCallback(unsigned button_idx, int button)
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
    if (button == HIGH)
    {
      if (state == State::PARAMETER)
      {
        myControlChange(midiChannel, CCvolume, clampToRange<int>(currentVolume, 1, 0, 127));
      }
    }
    if (button == HELD || button == HELD_REPEAT)
    {
      if (state == State::PARAMETER)
      {
        myControlChange(midiChannel, CCvolume, clampToRange<int>(currentVolume, 1, 0, 127));
      }
      break;

    case VOL_DOWN:
      if (button == HIGH)
      {
        if (state == State::PARAMETER)
        {
          myControlChange(midiChannel, CCvolume, clampToRange<int>(currentVolume, -1, 0, 127));
        }
      }
      if (button == HELD || button == HELD_REPEAT)
      {
        if (state == State::PARAMETER)
        {
          myControlChange(midiChannel, CCvolume, clampToRange<int>(currentVolume, -1, 0, 127));
        }
      }
      break;

      // Osc / Multi
    case BUTTON_1:
      if (button == HIGH)
      {
        singleLED(RED, 1);
        switch (state)
        {
        case State::PARAMETER:
          state = State::OSCPAGE; // show osc parameters
          singleLED(RED, 1);
          break;
        case State::OSCPAGE:
          state = State::OSCPAGE2;
          singleLED(RED, 1);
          break;
        case State::OSCPAGE2:
          state = State::PARAMETER;
          singleLED(OFF, 1);
          break;
        default:
          break;
        }
      }
      if (button == HELD)
      {
        singleLED(GREEN, 1);
      }
      break;

      // Osc Mod / Arp
    case BUTTON_2:
      if (button == HIGH)
      {
        switch (state)
        {
        case State::PARAMETER:
          state = State::OSCMODPAGE; // show osc mod parameters
          singleLED(RED, 2);
          break;
        case State::OSCPAGE:
          state = State::OSCMODPAGE2;
          singleLED(RED, 2);
          break;
        case State::OSCMODPAGE2:
          state = State::PARAMETER;
          singleLED(OFF, 2);
          break;
        default:
          break;
        }
      }
      if (button == HELD)
      {
        singleLED(GREEN, 2);
      }
      break;

      // Filter / Seq
    case BUTTON_3:
      if (button == HIGH)
      {
        singleLED(RED, 3);
        loadPatchNames(decTempBankIndex());
        if (patches[0].patchUID > 0)
        {
          currentBankIndex = tempBankIndex;
          currentPatchIndex = 0;
          recallPatch(currentBankIndex, patches[0].patchUID);
        }
      }
      if (button == HELD)
      {
        singleLED(GREEN, 3);
      }
      break;

      // Filter Mod / MIDI
    case BUTTON_4:
      if (button == HIGH)
      {
        singleLED(RED, 4);
        loadPatchNames(incTempBankIndex());
        if (patches[0].patchUID > 0)
        {
          currentBankIndex = tempBankIndex;
          currentPatchIndex = 0;
          recallPatch(currentBankIndex, patches[0].patchUID);
        }
        if (button == HELD)
        {
          singleLED(GREEN, 4);
        }
        break;
      }
      // Amp / Settings
    case BUTTON_5:
      if (button == HIGH)
      {
        singleLED(RED, 5);
      }
      if (button == HELD)
      {
        switch (state)
        {
        case State::PARAMETER:
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
        default:
          break;
        }
        singleLED(GREEN, 5);
      }
      break;

      // FX / Panic
    case BUTTON_6:
      if (button == HIGH)
      {
        singleLED(RED, 6);
      }
      if (button == HELD)
      {
        // If Back button held, Panic - all notes off
        groupvec[activeGroupIndex]->allNotesOff();
        groupvec[activeGroupIndex]->closeEnvelopes();
        flashLED(GREEN, 6, 250);
      }
      break;

      // Save / Del
    case BUTTON_7:
      if (button == HIGH)
      {
        // SAVE
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
        //   patchName = patches.back().patchName;
        //   state = PATCH;
        //   savePatch(String(patches.back().patchNo).c_str(), getCurrentPatchData());
        //   showPatchPage(patches.back().patchNo, patches.back().patchName);
        //   patchNo = patches.back().patchNo;
        //   loadPatches(); // Get rid of pushed patch if it wasn't saved
        //   // setPatchesOrdering(patchNo);
        //   renamedPatch = "";
        //   state = PARAMETER;
        //   break;

        //   // From Encoder button
        //   //  case SAVE:
        //   //    showRenamingPage(patches.back().patchName);
        //   //    patchName = patches.back().patchName;
        //   //    state = PATCHNAMING;
        //   //    break;
        // }
      }
      if (button == HELD)
      {
        // DELETE
        switch (state)
        {
        case State::PARAMETER:
        case PATCH:
          state = DELETE;
          break;
        default:
          break;
        }
        singleLED(GREEN, 7);
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
        case State::PARAMETER:
          tempBankIndex = currentBankIndex;
          loadPatchNames(tempBankIndex);
          state = State::RECALL; // show patch list
          break;
        case State::RECALL:
          // If current patch is empty in this bank, go to previous bank with valid patch
          while (patches[0].patchUID == 0)
          {
            tempBankIndex = decTempBankIndex();
            loadPatchNames(tempBankIndex);
          }
          currentBankIndex = tempBankIndex;
          currentPatchIndex = 0;
          recallPatch(currentBankIndex, patches[0].patchUID);
          state = State::PARAMETER;
          singleLED(OFF, 8);
          break;
        default:
          break;
        }
      }
      if (button == HELD)
      {
        // Reinitialise to default patch
        state = REINITIALISE;
        reinitialisePatch();
        setCurrentPatchData();
        flashLED(GREEN, 8, 250);
      }
      break;
    }
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
    if (cc == filterfreq256)
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