/*
   MicroDexed - https://codeberg.org/positionhigh/MicroDexed-touch

   MicroDexed is a port of the Dexed sound engine
   (https://github.com/asb2m10/dexed) for the Teensy-3.5/3.6/4.x with audio shield.
   Dexed ist heavily based on https://github.com/google/music-synthesizer-for-android

   (c)2018-2022 H. Wirtz <wirtz@parasitstudio.de>
   (c)2021-2022 H. Wirtz <wirtz@parasitstudio.de>, M. Koslowski <positionhigh@gmx.de>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/
#ifndef Sequencer_h_
#define Sequencer_h_
#include <stdint.h>
#include <math.h>
#include <Wire.h>
#include "Parameters.h"

#define NUM_SEQ_TRACKS 6
#define NUM_SEQ_PATTERN 10
#define FILENAME_LEN 20

typedef struct sequencer_t
{
  bool midi_learn_active = false;
  float drums_volume;
  uint8_t active_track = 0;
  uint8_t menu;
  bool noteoffsent[NUM_SEQ_TRACKS] = {false, false,  false, false, false, false};
  uint8_t inst_dexed[NUM_SEQ_TRACKS] = { 0, 0, 1, 1 , 1, 1 };
  uint8_t step = 0;
  bool running = false;
  bool recording = false;
  bool smartfilter = true;
  uint8_t state_last_loadsave = 200;
  char name[FILENAME_LEN];
  char name_temp[FILENAME_LEN];
  uint8_t note_in;
  uint8_t note_in_velocity;
  int transpose;

  uint8_t chord_dexed_inst = 0;
  uint8_t chord_velocity = 60;
  uint8_t chord_key_ammount = 4;
  uint8_t element_shift = 0;
  int oct_shift = 0;
  uint8_t arp_style = 0; // up, down, up&down, random

  const uint8_t arps[6][23] = {
    { 0, 4, 7, 12, 16, 19, 24, 28, 31, 36, 40, 43, 48, 52, 55, 60, 64, 67, 72, 76, 79, 84, 0}, //major
    { 0, 3, 7, 12, 15, 19, 24, 27, 31, 36, 39, 43, 48, 51, 55, 60, 63, 67, 72, 75, 79, 84, 0}, //minor
    { 0, 4, 7, 10, 12, 16, 19, 22, 24, 28, 31, 34, 36, 40, 43, 46, 48, 52, 55, 58, 60, 64, 0}, //seventh
    { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64, 68, 72, 76, 80, 84, 0}, //augmented
    { 0, 3, 6, 12, 15, 18, 24, 27, 30, 36, 39, 42, 48, 51, 54, 60, 63, 66, 72, 75, 78, 84, 0}, //dim
    { 0, 4, 7, 11, 12, 16, 19, 23, 24, 28, 31, 35, 36, 40, 43, 47, 48, 52, 55, 59, 60, 64, 0}  //maj7
  };

  const char chord_names[7][4] = {
    {'M', 'a', 'j', ' '}, //major
    {'M', 'i', 'n', ' '},
    {'s', 'e', 'v', ' '},
    {'a', 'u', 'g', ' '},
    {'d', 'i', 'm', ' '},
    { 'M', 'a', 'j', '7'},
    { 'N', 'o', 'C', 'd'}
  };

  const char arp_style_names[4][3] = {
    { 'u', 'p', ' '},
    {'d', 'w', 'n'},
    {'u', '&', 'd'},
    {'R', 'N', 'D'}
  };
  int tempo_ms = 180000;
  int bpm = 120;
  uint8_t temp_select_menu;
  uint8_t temp_active_menu = 99;
  uint8_t chain_active_chainstep;
  uint8_t chain_length = 3; // 0 = 16 steps, 1 = 32 Steps, 2 = 46 Steps, 3 = 64 Steps
  uint8_t chain_active_step = 0;
  uint8_t prev_note[NUM_SEQ_TRACKS]; // note_offs for every (instr.) track
  uint8_t prev_vel[NUM_SEQ_TRACKS];
  uint8_t arp_step;
  uint8_t arp_note;
  uint8_t arp_chord = 6;
  bool arp_play_basenote = true;
  uint8_t arp_note_prev;
  uint8_t arp_octave;
  uint8_t arp_prev_oct;
  uint8_t arp_speed = 0;
  uint8_t arp_counter = 0;
  uint8_t arp_length = 8;
  uint8_t data_buffer[16] = { 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0 };
  uint8_t note_data[NUM_SEQ_PATTERN][16] = {
    { 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0 },
    { 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0 },
    { 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0 },
    { 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0 },
    { 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0 },
    { 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0 },
    { 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0 },
    { 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0 },
    { 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0 },
    { 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0 }
  };
  uint8_t vel[NUM_SEQ_PATTERN][16] = {
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
  };
  uint8_t content_type[NUM_SEQ_PATTERN] = { 0, 0, 0, 0 , 0, 0, 0 , 0 , 0 , 0 }; // 0 = track is Drumtrack, 1 = Instrumenttrack, 2 = Chord or Arpeggio
  uint8_t patternchain[4][NUM_SEQ_TRACKS] = {
    { 0 , 2 , 6 , 9 , 99, 99 },
    { 1 , 2 , 5 , 8 , 99, 99 },
    { 0 , 2 , 6 , 9 , 99, 99 },
    { 1 , 2 , 5 , 7 , 99, 99 }
  };
  uint8_t track_type[NUM_SEQ_TRACKS] = { 0, 0, 1, 1, 1, 1 }; // 0 = track is Drumtrack, 1 = Instrumenttrack, 2 = Chord, 3 = Arp
} sequencer_t;
#endif