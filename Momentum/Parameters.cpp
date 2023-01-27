#include "Parameters.h"
#include "Constants.h"

// Values below are just for initialising and will be changed when synth is initialised to current panel controls & EEPROM settings

// Globals used for Settings
byte midiChannel = MIDI_CHANNEL_OMNI;        //(EEPROM)
byte midiOutCh = 0;                          //(EEPROM)
midi::Thru::Mode MIDIThru = midi::Thru::Off; //(EEPROM)
String patchName = INITPATCHNAME;
boolean encCW = true; // This is to set the encoder to increment when turned CW - Settings Option
boolean vuMeter = false;
int8_t tuningCents = 0;
float tuningCentsFrac = 1.0f;

float midiClkTimeInterval = 0.0f;

String oscLFOTimeDivStr = ""; // For display
// Exponential envelopes
int8_t envTypeAmp = -128;  // Linear
int8_t envTypeFilt = -128; // Linear
// Glide shape
int8_t glideShape = 1; // Exp

uint8_t currentVolume = 63;

int8_t charCursor = 0;
int8_t patchNameCursor = 0;