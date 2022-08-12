#pragma once
#include <stdint.h>
#include <MIDI.h>

// Globals used for Settings
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

extern uint8_t currentVolume;

// Patch parameters to store the relative 7-bit values (Cutoff freq 8-bit) for patch store and modification by the encoders
// Initialised with a default patch to store when there are no patches on SD card
typedef struct PatchStruct
{
    uint32_t UID = 0;
    char PatchName[64] = "Solina";
    uint8_t OscLevelA = 127;
    uint8_t OscLevelB = 127;
    uint8_t NoiseLevel = 63; // Centre off
    uint8_t Unison = 0;
    uint8_t OscFX = 0;
    uint8_t Detune = 20;
    uint8_t LFOSyncFreq = 0;
    uint8_t MidiClkTimeInterval = 0;
    uint8_t LFOTempoValue = 0;
    uint8_t KeyTracking = 0;
    uint8_t Glide = 0;
    uint8_t PitchA = 0;
    uint8_t PitchB = 8;
    uint8_t WaveformA = 75;
    uint8_t WaveformB = 75;
    uint8_t PWMSourceA = 0;
    uint8_t PWMSourceB = 0;
    uint8_t PWMA_Amount = 63;
    uint8_t PWMB_Amount = 63;
    uint8_t PWMRateA = 10;
    uint8_t PWMRateB = 10;
    uint8_t PWA_Amount = 0;
    uint8_t PWB_Amount = 0;
    uint8_t FilterRes = 10;
    uint8_t FilterFreq = 230;
    uint8_t FilterMixer = 0;
    uint8_t FilterEnv = 89;
    uint8_t PitchLFOAmt = 0;
    uint8_t PitchLFORate = 0;
    uint8_t PitchLFOWaveform = 0;
    uint8_t PitchLFORetrig = 0;
    uint8_t PitchLFOMidiClkSync = 0;
    uint8_t FilterLFORate = 0;
    uint8_t FilterLFORetrig = 0;
    uint8_t FilterLFOMidiClkSync = 0;
    uint8_t FilterLfoAmt = 0;
    uint8_t FilterLFOWaveform = 0;
    uint8_t FilterAttack = 0;
    uint8_t FilterDecay = 20;
    uint8_t FilterSustain = 80;
    uint8_t FilterRelease = 50;
    uint8_t Attack = 0;
    uint8_t Decay = 20;
    uint8_t Sustain = 80;
    uint8_t Release = 25;
    uint8_t EffectAmt = 20;
    uint8_t EffectMix = 105;
    uint8_t PitchEnv = 63; // Centre off
    uint8_t VelocitySensitivity = 0;
    uint8_t ChordDetune = 0;
    uint8_t MonophonicMode = 0;
    uint8_t FilterEnvShape = 0;
    uint8_t AmpEnvShape = 0;
    uint8_t GlideShape = 0;
    uint8_t PitchBend = 12;
    uint8_t ModWheelDepth = 1;
} PatchStruct;

typedef struct SequenceStruct
{
    uint32_t UID = 0;
    char SequenceName[64] = "Sequence 1";
} SequenceStruct;

typedef struct PatchesInPerfStruct
{
    char Bank[20] = "";
    PatchStruct patch;
    uint8_t midiCh = 0;
    uint8_t min = 0;
    uint8_t max = 127;
} PatchesInPerfStruct;

typedef enum PerformanceMode
{
    Single,
    Layer,
    Split
} PerformanceMode;

typedef struct PerformanceStruct
{
    uint32_t UID = 0;
    char PerformanceName[64] = "Performance 1";
    PerformanceMode mode = PerformanceMode::Single;
    PatchesInPerfStruct pp[];
} PerformanceStruct;