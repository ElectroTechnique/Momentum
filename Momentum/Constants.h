#pragma once

#include <stdint.h>
#include "Arduino.h"

#define PWMWAVEFORM WAVEFORM_TRIANGLE
#define BANKS_LIMIT 8          // 8 banks
#define PATCHES_LIMIT 128      // 128 patches per bank
#define PERFORMANCES_LIMIT 128 // 128 performances
#define SEQUENCES_LIMIT 128    // 128 sequences

// Used in myControlChange for 8 bit control of filter cutoff freq from front panel
#define filterfreq256 128
#define patchselect 129
#define savepatch 130
#define renamepatch 131
#define deletepatch 132
#define temposync 133
#define ampenvshape 134
#define filterenvshape 135
#define glideshape 136
#define pitchbendrange 137
#define modwheeldepth 138
#define savepatchselect 139
#define savebankselect 140
#define MIDIChIn 141
#define MIDIChOut 142
#define MIDIThruMode 143
#define PerfSelect 144
#define PerfEdit 145

    const static char *SEQUENCE_FOLDER_NAME = "Sequences";
const static char *SEQUENCE_FOLDER_NAME_SLASH = "Sequences/";

const static char *BANKS_FILE_NAME = "Banknames";
const static char *BANK_FOLDER_NAMES[] = {"Bank1", "Bank2", "Bank3", "Bank4", "Bank5", "Bank6", "Bank7", "Bank8"};
const static char *BANK_FOLDER_NAMES_SLASH[] = {"Bank1/", "Bank2/", "Bank3/", "Bank4/", "Bank5/", "Bank6/", "Bank7/", "Bank8/"};
// Defaults to store on blank SD card
const static char *BANKS_FILE_CONTENTS = "{\"Bank1\": \"Strings\",\"Bank2\": \"Basses\", \"Bank3\": \"Keys\",\"Bank4\": \"FX\",\"Bank5\": \"User 1\",\"Bank6\": \"User 2\",\"Bank7\": \"User 3\",\"Bank8\": \"User 4\"}";

// Friendly bank names from Banknames file - defaults
static char *bankNames[] = {"Strings", "Basses", "Keys", "FX", "User 1", "User 2", "User 3", "User 4"};

const static char *PERFORMANCE_FOLDER_NAME = "Performances";
const static char *PERFORMANCE_FOLDER_NAME_SLASH = "Performances/";

const static char *EnvShapeStr[] = {"Lin", "Exp -8", "Exp -7", "Exp -6", "Exp -5", "Exp -4", "Exp -3", "Exp -2", "Exp -1", "Exp 0", "Exp +1", "Exp +2", "Exp +3", "Exp +4", "Exp +5", "Exp +6", "Exp +7", "Exp +8"};
const static char *GlideShapeStr[] = {"Lin", "Exp"};
const static char *MonophonicStr[] = {"Off", "Last", "First", "Highest", "Lowest"};

#define TOTALCHARS 64
const static char CHARACTERS[TOTALCHARS] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', ' ', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', ' ', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0'};

extern const char *VERSION;
extern const float PROGMEM FILTERFREQS256[256];
extern const float POWER[128];
extern const float NOTEFREQS[128];
extern const uint32_t ENVTIMES[128];
extern const float LFOTEMPO[128];
extern const String LFOTEMPOSTR[128];
extern const uint32_t OSCMIXA[128];
extern const uint32_t OSCMIXB[128];
extern const int PITCH[128];
extern const float KEYTRACKINGAMT[128];
extern const float DIV8192;
extern const float DIV127;
extern const float DIV12;
extern const float DIV24;
extern const float DIV1270;
extern const float LINEAR[128];
extern const float LINEARCENTREZERO[128];
extern const int8_t ENVSHAPE[18];

extern const float BANDPASS;
extern const float LOWPASS;
extern const float HIGHPASS;
extern const float LINEAR_FILTERMIXER[128];
extern const int16_t PROGMEM PARABOLIC_WAVE[256];
extern const int16_t PROGMEM HARMONIC_WAVE[256];
extern const int16_t PROGMEM PPG_WAVE[256];
extern const float AWFREQ;
extern const float PWMRATE[128];
extern const float PITCHLFOOCTAVERANGE;
extern const uint32_t MINUNISONVOICES;
extern const float LFOMAXRATE;
#define PWMSOURCELFO 0
#define PWMSOURCEFENV 1
#define PWMSOURCEFIXED 2
#define OSCFXOFF 0
#define OSCFXXOR 1
#define OSCFXXMOD 2
extern const float ONE;
extern const float SGTL_MAXVOLUME;
extern const float WAVEFORMLEVEL;
extern const float VOICEMIXERLEVEL;
extern const float MAX_VOL;
extern const float UNISONVOICEMIXERLEVEL;
extern const float UNISONNOISEMIXERLEVEL;
extern const float OSCMODMIXERMAX;
extern const float FILTERMODMIXERMAX;
extern const float GLIDEFACTOR;
extern const uint32_t NO_OF_PARAMS;
extern const char *INITPATCHNAME;
extern const uint32_t HOLD_DURATION;
extern const uint32_t CLICK_DURATION;
extern const float ENSEMBLE_LFO[128];
extern const char *INITPATCH;

typedef enum State
{
  MAIN,           // The main page for displaying the current patch and control (parameter) changes
  PATCHLIST,      // Patches list
  SAVE,           // Save patch page
  REINITIALISE,   // Reinitialise message
  PATCHSAVING,    // Patch naming page
  DELETE,         // Delete patch page
  DELETEMSG,      // Delete patch message page
  SETTINGS,       // Settings page
  SETTINGSVALUE,  // Settings page
  OSCPAGE1,       // Osc parameters 1
  OSCPAGE2,       // Osc parameters 2
  OSCPAGE3,       // Osc parameters 2
  OSCMODPAGE1,    // Osc Mod parameters 1
  OSCMODPAGE2,    // Osc Mod parameters 2
  OSCMODPAGE3,    // Osc Mod parameters 3
  OSCMODPAGE4,    // Osc Mod parameters 4
  FILTERPAGE1,    // Filter parameters 1
  FILTERPAGE2,    // Filter parameters 2
  FILTERMODPAGE1, // Filter Mod parameters 1
  FILTERMODPAGE2, // Filter Mod parameters 2
  FILTERMODPAGE3, // Filter Mod parameters 3
  AMPPAGE1,       // Amplifier parameters 1
  AMPPAGE2,       // Amplifier parameters 2
  FXPAGE,         // FX parameters
  MIDIPAGE,       // MIDI parameters
  ARPPAGE,        // Arpeggiator parameters
  SEQPAGE,        // Sequencer parameters
  PERFORMANCEPAGE // Performance parameters
} State;