#pragma once
#include <stdint.h>
#include <MIDI.h>

// Globals used for LCD Settings
extern byte midiChannel;
extern byte midiOutCh;
extern midi::Thru::Mode MIDIThru;
extern String patchName;
extern boolean encCW;
extern boolean vuMeter;

// Global patch modifiers
extern float lfoSyncFreq;
extern float midiClkTimeInterval;
extern float lfoTempoValue;
extern int pitchBendRange;
extern float modWheelDepth;
extern String oscLFOTimeDivStr;
extern int velocitySens;
// Exponential envelopes
extern int8_t envTypeAmp;
extern int8_t envTypeFilt;
// Glide shape
extern int8_t glideShape;
// Encoders
extern int16_t encTRValue;
extern int16_t encBRValue;
extern int16_t encTLValue;
extern int16_t encBLValue;