/*
This code is taken from:

   MicroDexed - https://codeberg.org/positionhigh/MicroDexed-touch

   MicroDexed is a port of the Dexed sound engine
   (https://github.com/asb2m10/dexed) for the Teensy-3.5/3.6/4.x with audio shield.
   Dexed ist heavily based on https://github.com/google/music-synthesizer-for-android

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

   Modified by Electrotechnique 2022
*/

#include "Sequencer.h"

extern void UI_func_seq_pattern_editor(uint8_t);
extern void UI_func_arpeggio(uint8_t);
extern const char *seq_find_shortname(uint8_t);

extern void myNoteOff(byte channel, byte note, byte velocity);
extern void myNoteOn(byte channel, byte note, byte velocity);

bool interrupt_swapper = false;

sequencer_t seq;

void seq_live_recording()
{
  // record to sequencer if recording is active
  if (seq.note_in > 0 && seq.recording == true)
  {
    seq.note_data[seq.active_track][seq.step] = seq.note_in;
    seq.vel[seq.active_track][seq.step] = seq.note_in_velocity;

    seq.note_in = 0;
    seq.note_in_velocity = 0;
  }
}

void noteOnRoutine()
{
  if (seq.note_in > 0 && seq.note_in < 62 && seq.recording == false)
  {
    myNoteOff(midiChannel, seq.note_data[3][seq.step] + seq.transpose, 0);
    myNoteOff(midiChannel, seq.note_data[3][seq.step - 1] + seq.transpose, 0);
    if (seq.note_in > 65)
      seq.note_in = seq.note_in - 12;
    seq.transpose = seq.note_in % 12;
    seq.transpose = seq.transpose - 12;
    seq.note_in = 0;
  }

  seq_live_recording();
  for (uint8_t d = 0; d < NUM_SEQ_TRACKS; d++)
  {
    if (seq.patternchain[seq.chain_active_step][d] < NUM_SEQ_PATTERN) // sequence not empty or muted
    {

      if (seq.note_data[seq.patternchain[seq.chain_active_step][d]][seq.step] > 0) // instrument track
      {
        if (seq.track_type[d] == TrackType::INSTRUMENT ||
            (seq.track_type[d] == TrackType::ARP && seq.arp_play_basenote))
        {
          if (seq.note_data[seq.patternchain[seq.chain_active_step][d]][seq.step] != 130)
          {
            myNoteOn(midiChannel, seq.note_data[seq.patternchain[seq.chain_active_step][d]][seq.step], seq.vel[seq.patternchain[seq.chain_active_step][d]][seq.step]);
            seq.prev_note[d] = seq.note_data[seq.patternchain[seq.chain_active_step][d]][seq.step];
            seq.prev_vel[d] = seq.vel[seq.patternchain[seq.chain_active_step][d]][seq.step];
          }
        }
        else if (seq.track_type[d] == TrackType::CHORD) // Chords
        {
          if (seq.vel[seq.patternchain[seq.chain_active_step][d]][seq.step] > 199)
          {

            // myNoteOn(midiChannel, seq.note_data[ seq.patternchain[seq.chain_active_step][d] ][seq.step], seq.chord_velocity);  // basenote

            for (uint8_t x = seq.element_shift; x < seq.element_shift + seq.chord_key_ammount; x++) // play chord notes
            {
              myNoteOn(midiChannel, seq.note_data[seq.patternchain[seq.chain_active_step][d]][seq.step] + (seq.oct_shift * 12) + seq.arps[seq.vel[seq.patternchain[seq.chain_active_step][d]][seq.step] - 200][x], seq.chord_velocity);
            }
            seq.prev_note[d] = seq.note_data[seq.patternchain[seq.chain_active_step][d]][seq.step] + (seq.oct_shift * 12);
            seq.prev_vel[d] = seq.vel[seq.patternchain[seq.chain_active_step][d]][seq.step];
          }
        }
        if (seq.track_type[d] == TrackType::ARP)
        { // Arp
          seq.arp_step = 0;
          seq.arp_counter = 0;
          seq.arp_note = seq.note_data[seq.patternchain[seq.chain_active_step][d]][seq.step] + (seq.oct_shift * 12);
          seq.arp_chord = seq.vel[seq.patternchain[seq.chain_active_step][d]][seq.step] - 200;
        }
      }

      // after here not triggered by a key input -  arp only
      if (seq.track_type[d] == TrackType::ARP)
      { // Arp
        if (seq.arp_speed == 0 || (seq.arp_speed == 1 && seq.arp_counter == 0))
        {
          {
            if (seq.arp_style == 0)
            { // arp up
              myNoteOn(midiChannel, seq.arp_note + seq.arps[seq.arp_chord][seq.arp_step + seq.element_shift],
                       seq.chord_velocity);
              seq.arp_note_prev = seq.arp_note + seq.arps[seq.arp_chord][seq.arp_step + seq.element_shift];
            }
            else if (seq.arp_style == 1)
            { // arp down
              myNoteOn(midiChannel, seq.arp_note + seq.arps[seq.arp_chord][seq.arp_length - seq.arp_step + seq.element_shift],
                       seq.chord_velocity);
              seq.arp_note_prev = seq.arp_note + seq.arps[seq.arp_chord][seq.arp_length - seq.arp_step + seq.element_shift];
            }
            else if (seq.arp_style == 2)
            { // arp up & down
              if (seq.arp_step <= seq.arp_length)
              {
                myNoteOn(midiChannel, seq.arp_note + seq.arps[seq.arp_chord][seq.arp_step], seq.chord_velocity);
                seq.arp_note_prev = seq.arp_note + seq.arps[seq.arp_chord][seq.arp_step];
              }
              else
              {
                myNoteOn(midiChannel, seq.arp_note + seq.arps[seq.arp_chord][seq.arp_length * 2 - seq.arp_step],
                         seq.chord_velocity);
                seq.arp_note_prev = seq.arp_note + seq.arps[seq.arp_chord][seq.arp_length * 2 - seq.arp_step];
              }
            }
            else if (seq.arp_style == 3)
            {                   // arp random
              uint8_t rnd1 = 3; // random(seq.arp_length); TODO
              myNoteOn(midiChannel, seq.arp_note + seq.arps[seq.arp_chord][rnd1 + seq.element_shift] + (seq.oct_shift * 12),
                       seq.chord_velocity);
              seq.arp_note_prev = seq.arp_note + seq.arps[seq.arp_chord][rnd1 + seq.element_shift] + (seq.oct_shift * 12);
            }
          }
        }
      }
    }
    seq.noteoffsent[d] = false;
  }
  seq.arp_counter++;
  seq.step++;
  if (seq.arp_speed == 0) // Arp Speed 1/16
  {
    seq.arp_step++;
  }
  else
  {
    // Arp Speed 1/8
    if (seq.arp_counter > 1)
    {
      seq.arp_counter = 0;
      seq.arp_step++;
    }
  }

  if (seq.arp_style != 2)
  {

    if ((seq.arp_step > 1 && seq.arps[seq.arp_chord][seq.arp_step] == 0) || seq.arp_step == seq.arp_length)
    {
      seq.arp_step = 0;
    }
  }
  if (seq.arp_style == 1 || seq.arp_style == 2)
  {
    if (seq.arp_length == 0)
      seq.arp_length = 9;
  }
  if (seq.arp_style == 2) // only for up&down
  {
    if ((seq.arp_step > 1 && seq.arps[seq.arp_chord][seq.arp_step] == 0) || seq.arp_step == seq.arp_length * 2)
    {
      seq.arp_step = 0;
    }
  }
  if (seq.step > 15)
  {
    seq.step = 0;
    if (seq.chain_length > 0)
    {
      seq.chain_active_step++;
      if (seq.chain_active_step > seq.chain_length)
      {
        seq.chain_active_step = 0;
      }
    }
  }
}

void noteOffRoutine()
{
  seq_live_recording();
  for (uint8_t d = 0; d < NUM_SEQ_TRACKS; d++)
  {
    if (seq.noteoffsent[d] == false)
    {
      if (seq.prev_note[d] > 0 && seq.track_type[d] > 0)
      {
        if (seq.note_data[seq.patternchain[seq.chain_active_step][d]][seq.step] != 130)
        {
          myNoteOff(midiChannel, seq.prev_note[d], 0);
          seq.noteoffsent[d] = true;
        }
        if (seq.track_type[d] == TrackType::CHORD)
        { // Chords
          if (seq.prev_vel[d] > 199)
          {
            for (uint8_t x = seq.element_shift; x < seq.element_shift + seq.chord_key_ammount; x++) // play chord notes
            {
              myNoteOff(midiChannel, seq.prev_note[d] + seq.arps[seq.prev_vel[d] - 200][x], 0);
              seq.noteoffsent[d] = true;
            }
          }
        }
        else if (seq.track_type[d] == TrackType::ARP)
        { // Arp
          myNoteOff(midiChannel, seq.arp_note_prev, 0);
          seq.noteoffsent[d] = true;
        }
      }
    }
  }
}

void sequencer()
{ // Runs in Interrupt Timer. Switches between the Noteon and Noteoff Task, each cycle
  Serial.println("sequencer()");
  interrupt_swapper = !interrupt_swapper;
  if (interrupt_swapper)
    noteOnRoutine();
  else
    noteOffRoutine();
}
