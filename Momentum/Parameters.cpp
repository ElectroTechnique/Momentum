#include "Parameters.h"
#include "Constants.h"

// Values below are just for initialising and will be changed when synth is initialised to current panel controls & EEPROM settings

// Globals used for OLED Settings
byte midiChannel = MIDI_CHANNEL_OMNI;        //(EEPROM)
byte midiOutCh = 0;                          //(EEPROM)
midi::Thru::Mode MIDIThru = midi::Thru::Off; //(EEPROM)
String patchName = INITPATCHNAME;
boolean encCW = true; // This is to set the encoder to increment when turned CW - Settings Option
boolean vuMeter = false;

// Global patch modifiers
float lfoSyncFreq = 1.0f;
float midiClkTimeInterval = 0.0f;
float lfoTempoValue = 1.0f;
int pitchBendRange = 12;
float modWheelDepth = 0.2f;
String oscLFOTimeDivStr = ""; // For display
int velocitySens = 0;         // Default off - settings option
// Exponential envelopes
int8_t envTypeAmp = -128;  // Linear
int8_t envTypeFilt = -128; // Linear
// Glide shape
int8_t glideShape = 1; // Exp

uint8_t currentVolume = 63;

int8_t chosenChar = 0;