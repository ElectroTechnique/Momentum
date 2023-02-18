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

   Modified by Electrotechnique 2023

Note value  Significance
130         Held

Vel Value   Significance
>199        Note on for chords?
*/

// Janelia Array available in libraries manager
#include <Array.h>
#include "Constants.h"
#include "Utils.h"
#include <ArduinoJson.h>
#include "Parameters.h"

#define SEQ_PATTERN_LEN 16
#define NUM_SEQ_PATTERN 4

extern void myNoteOff(byte channel, byte note, byte velocity);
extern void myNoteOnSeq(byte channel, byte note, byte velocity);

//  uint8_t midi_bpm_counter = 0;
//  uint8_t midi_bpm = 0;
//  int16_t _midi_bpm = -1;
//  elapsedMillis midi_bpm_timer;

//  1 = Instrumenttrack, 2 = Chord, 3 = Arp
typedef enum TrackType
{
    INSTRUMENT = 1,
    CHORD = 2,
    ARP = 3
} TrackType;

const char chord_names[7][4] = {
    {'M', 'a', 'j', ' '}, // major
    {'M', 'i', 'n', ' '},
    {'s', 'e', 'v', ' '},
    {'a', 'u', 'g', ' '},
    {'d', 'i', 'm', ' '},
    {'M', 'a', 'j', '7'},
    {'N', 'o', 'C', 'd'}};

const char arp_style_names[4][3] = {
    {'u', 'p', ' '},
    {'d', 'w', 'n'},
    {'u', '&', 'd'},
    {'R', 'N', 'D'}};

const uint8_t arps[6][23] = {
    {0, 4, 7, 12, 16, 19, 24, 28, 31, 36, 40, 43, 48, 52, 55, 60, 64, 67, 72, 76, 79, 84, 0}, // major
    {0, 3, 7, 12, 15, 19, 24, 27, 31, 36, 39, 43, 48, 51, 55, 60, 63, 67, 72, 75, 79, 84, 0}, // minor
    {0, 4, 7, 10, 12, 16, 19, 22, 24, 28, 31, 34, 36, 40, 43, 46, 48, 52, 55, 58, 60, 64, 0}, // seventh
    {0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64, 68, 72, 76, 80, 84, 0}, // augmented
    {0, 3, 6, 12, 15, 18, 24, 27, 30, 36, 39, 42, 48, 51, 54, 60, 63, 66, 72, 75, 78, 84, 0}, // dim
    {0, 4, 7, 11, 12, 16, 19, 23, 24, 28, 31, 35, 36, 40, 43, 47, 48, 52, 55, 59, 60, 64, 0}  // maj7
};

typedef struct SequenceStruct
{
    uint32_t UID = 0;
    String SequenceName = F("-Empty-");
    float bpm = 120.0f;
    uint8_t length = SEQ_PATTERN_LEN;
    uint8_t Notes[NUM_SEQ_PATTERN][SEQ_PATTERN_LEN];
    uint8_t Velocities[NUM_SEQ_PATTERN][SEQ_PATTERN_LEN];
    uint8_t patternchain[NUM_SEQ_PATTERN] = {0, 1, 2, 3};
    uint8_t track_type = INSTRUMENT; //  1 = Instrumenttrack, 2 = Chord, 3 = Arp
    uint8_t active_track = 0;
    boolean noteoffsent = false;
    uint8_t step = 0;
    boolean running = false;
    boolean recording = false;

    uint8_t note_in;
    uint8_t note_in_velocity;
    int8_t transpose;

    uint8_t chord_velocity = 60;
    uint8_t chord_key_amount = 4;
    uint8_t element_shift = 0;
    int8_t oct_shift = 0;
    uint8_t arp_style = 0;       // up, down, up&down, random
    uint32_t tempo_us = 125'000; // 60s/bpm

    uint8_t chain_active_chainstep;
    uint8_t chain_length = 3; // 0 = 16 steps, 1 = 32 Steps, 2 = 48 Steps, 3 = 64 Steps
    uint8_t chain_active_step = 0;
    uint8_t prev_note; // note_offs for every (instr.) track
    uint8_t prev_vel;
    uint8_t arp_step;
    uint8_t arp_note;
    uint8_t arp_chord = 6;
    boolean arp_play_basenote = true;
    uint8_t arp_note_prev;
    uint8_t arp_octave;
    uint8_t arp_prev_oct;
    uint8_t arp_speed = 0;
    uint8_t arp_counter = 0;
    uint8_t arp_length = 8;
} SequenceStruct;

uint8_t currentSequenceIndex = 0;

boolean seqMidiSync = false;

SequenceStruct currentSequence;

String sequences[SEQUENCES_LIMIT];

FLASHMEM void
concatSequenceFolderAndFilename(uint8_t filename, char *result)
{
    char buf[20];
    strcpy(result, SEQUENCE_FOLDER_NAME_SLASH); // copy string one into the result.
    strcat(result, utoa(filename, buf, 10));    // append string two to the result
}

// Filename is Index
FLASHMEM boolean loadSequence(uint8_t filename)
{
    // Open file for reading - UID is Filename
    char result[30];
    concatSequenceFolderAndFilename(filename, result);
    File file = SD.open(result);

    // Allocate a temporary JsonDocument
    // Don't forget to change the capacity to match your requirements.
    // Use https://arduinojson.org/v6/assistant to compute the capacity.
    StaticJsonDocument<3000> doc;

    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, file);
    if (error)
    {
        Serial.print(F("loadSequence() - Failed to read file:"));
        Serial.println(filename);
        return false;
    }
    // Copy values from the JsonDocument to the SequenceStruct
    currentSequence.SequenceName = doc["Name"].as<String>();
    currentSequence.UID = doc["UID"];
    currentSequence.bpm = doc["bpm"];
    currentSequence.length = doc["Length"];

    JsonArray Notes = doc["Notes"];
    JsonArray Vel = doc["Vel"];

    for (uint8_t i = 0; i < NUM_SEQ_PATTERN; i++)
    {
        JsonArray Notes_0 = Notes[i];
        JsonArray Vel_0 = Vel[i];
        for (uint8_t j = 0; j < SEQ_PATTERN_LEN; j++)
        {
            currentSequence.Notes[i][j] = Notes_0[j];
            currentSequence.Velocities[i][j] = Vel_0[j];
        }
    }
    file.close();
    return true;
}

FLASHMEM void saveSequence()
{
    StaticJsonDocument<3000> doc;
    // Need to generate a new UID as the sequence settings may have changed if overwriting an existing sequence
    String output;
    doc["Name"] = currentSequence.SequenceName;
    doc["Length"] = currentSequence.length;
    doc["bpm"] = currentSequence.bpm;
    JsonArray Pattern = doc.createNestedArray("Notes");
    JsonArray PatternVel = doc.createNestedArray("Vel");

    for (uint8_t i = 0; i < NUM_SEQ_PATTERN; i++)
    {
        JsonArray Notes[i] = Pattern.createNestedArray();
        JsonArray Vel[i] = PatternVel.createNestedArray();
        for (uint8_t j = 0; j < SEQ_PATTERN_LEN; j++)
        {
            Notes[i].add(currentSequence.Notes[i][j]);
            Vel[i].add(currentSequence.Velocities[i][j]);
        }
    }
    serializeJson(doc, output);   // Generate complete JSON to save
    doc["UID"] = getHash(output); // Insert UID
    char result[30];
    concatSequenceFolderAndFilename(currentSequenceIndex + 1, result);
    File file = SD.open(result, FILE_WRITE);
    if (!file)
    {
        Serial.println(F("Failed to create file"));
        return;
    }
    if (serializeJson(doc, file) == 0)
    {
        Serial.println(F("Failed to write to file"));
        return;
    }
}

FLASHMEM char *getSequenceName(File file)
{
    // Allocate a temporary JsonDocument
    // Don't forget to change the capacity to match your requirements.
    // Use https://arduinojson.org/v6/assistant to compute the capacity.
    StaticJsonDocument<3000> doc;

    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, file);
    if (error)
    {
        Serial.println(F("getSequenceName() - Failed to read file"));
        return "Failed to read file";
    }
    // Copy values from the JsonDocument to the SequenceStruct
    return doc["Name"];
}

FLASHMEM void loadSequenceNames()
{
    if (!cardStatus)
        return;
    File sequenceDir = SD.open(SEQUENCE_FOLDER_NAME_SLASH);
    File sequenceFile;
    for (uint8_t i = 0; i < SEQUENCES_LIMIT; i++)
    {
        sequences[i] = {F("-Empty-")};
    }

    while (sequenceFile = sequenceDir.openNextFile())
    {
        if (!sequenceFile)
            break;
        uint8_t number = strtoul(sequenceFile.name(), NULL, 0);
        if (number < 1 || number > 128)
        {
            Serial.print(F("Sequence number is outside 1-128:"));
            Serial.println(number);
        }
        else
        {
            sequences[number - 1] = getSequenceName(sequenceFile);
        }
    }
    sequenceFile.close();
}

FLASHMEM uint8_t incSequenceIndex()
{
    if (currentSequenceIndex < SEQUENCES_LIMIT - 1)
    {
        return ++currentSequenceIndex;
    }
    else
    {
        // Go back to Start from front of array
        currentSequenceIndex = 0;
        return currentSequenceIndex;
    }
}

FLASHMEM uint8_t decSequenceIndex()
{
    if (currentSequenceIndex > 0)
    {
        return --currentSequenceIndex;
    }
    else
    {
        // Go to back of array
        currentSequenceIndex = SEQUENCES_LIMIT - 1;
        return currentSequenceIndex;
    }
}

//////////// Sequencer /////////////
bool interrupt_swapper = false;

void seq_live_recording()
{
    // record to sequencer if recording is active
    if (currentSequence.note_in > 0 && currentSequence.recording == true)
    {
        currentSequence.Notes[currentSequence.active_track][currentSequence.step] = currentSequence.note_in;
        currentSequence.Velocities[currentSequence.active_track][currentSequence.step] = currentSequence.note_in_velocity;

        currentSequence.note_in = 0;
        currentSequence.note_in_velocity = 0;
    }
}

void noteOnRoutine()
{
    if (currentSequence.note_in > 0 && currentSequence.note_in < 62 && currentSequence.recording == false)
    {
        myNoteOff(midiChannel, currentSequence.Notes[3][currentSequence.step] + currentSequence.transpose, 0);
        myNoteOff(midiChannel, currentSequence.Notes[3][currentSequence.step - 1] + currentSequence.transpose, 0);
        if (currentSequence.note_in > 65)
            currentSequence.note_in = currentSequence.note_in - 12;
        currentSequence.transpose = currentSequence.note_in % 12;
        currentSequence.transpose = currentSequence.transpose - 12;
        currentSequence.note_in = 0;
    }

    seq_live_recording();

    if (currentSequence.Notes[currentSequence.patternchain[currentSequence.chain_active_step]][currentSequence.step] > 0) // instrument track
    {
        if (currentSequence.track_type == TrackType::INSTRUMENT || (currentSequence.track_type == TrackType::ARP && currentSequence.arp_play_basenote))
        {
            // 130 signifies latched note
            if (currentSequence.Notes[currentSequence.patternchain[currentSequence.chain_active_step]][currentSequence.step] != 130)
            {
                myNoteOnSeq(midiChannel, currentSequence.Notes[currentSequence.patternchain[currentSequence.chain_active_step]][currentSequence.step], currentSequence.Velocities[currentSequence.patternchain[currentSequence.chain_active_step]][currentSequence.step]);
                currentSequence.prev_note = currentSequence.Notes[currentSequence.patternchain[currentSequence.chain_active_step]][currentSequence.step];
                currentSequence.prev_vel = currentSequence.Velocities[currentSequence.patternchain[currentSequence.chain_active_step]][currentSequence.step];
            }
        }
        else if (currentSequence.track_type == TrackType::CHORD) // Chords
        {
            if (currentSequence.Velocities[currentSequence.patternchain[currentSequence.chain_active_step]][currentSequence.step] > 199)
            {

                // myNoteOnSeq(midiChannel, currentSequence.Notes[ currentSequence.patternchain[currentSequence.chain_active_step]][currentSequence.step], currentSequence.chord_velocity);  // basenote

                for (uint8_t x = currentSequence.element_shift; x < currentSequence.element_shift + currentSequence.chord_key_amount; x++) // play chord notes
                {
                    myNoteOnSeq(midiChannel, currentSequence.Notes[currentSequence.patternchain[currentSequence.chain_active_step]][currentSequence.step] + (currentSequence.oct_shift * 12) + arps[currentSequence.Velocities[currentSequence.patternchain[currentSequence.chain_active_step]][currentSequence.step] - 200][x], currentSequence.chord_velocity);
                }
                currentSequence.prev_note = currentSequence.Notes[currentSequence.patternchain[currentSequence.chain_active_step]][currentSequence.step] + (currentSequence.oct_shift * 12);
                currentSequence.prev_vel = currentSequence.Velocities[currentSequence.patternchain[currentSequence.chain_active_step]][currentSequence.step];
            }
        }
        if (currentSequence.track_type == TrackType::ARP)
        { // Arp
            currentSequence.arp_step = 0;
            currentSequence.arp_counter = 0;
            currentSequence.arp_note = currentSequence.Notes[currentSequence.patternchain[currentSequence.chain_active_step]][currentSequence.step] + (currentSequence.oct_shift * 12);
            currentSequence.arp_chord = currentSequence.Velocities[currentSequence.patternchain[currentSequence.chain_active_step]][currentSequence.step] - 200;
        }
    }

    // after here not triggered by a key input -  arp only
    if (currentSequence.track_type == TrackType::ARP)
    { // Arp
        if (currentSequence.arp_speed == 0 || (currentSequence.arp_speed == 1 && currentSequence.arp_counter == 0))
        {
            {
                if (currentSequence.arp_style == 0)
                { // arp up
                    myNoteOnSeq(midiChannel, currentSequence.arp_note + arps[currentSequence.arp_chord][currentSequence.arp_step + currentSequence.element_shift],
                                currentSequence.chord_velocity);
                    currentSequence.arp_note_prev = currentSequence.arp_note + arps[currentSequence.arp_chord][currentSequence.arp_step + currentSequence.element_shift];
                }
                else if (currentSequence.arp_style == 1)
                { // arp down
                    myNoteOnSeq(midiChannel, currentSequence.arp_note + arps[currentSequence.arp_chord][currentSequence.arp_length - currentSequence.arp_step + currentSequence.element_shift],
                                currentSequence.chord_velocity);
                    currentSequence.arp_note_prev = currentSequence.arp_note + arps[currentSequence.arp_chord][currentSequence.arp_length - currentSequence.arp_step + currentSequence.element_shift];
                }
                else if (currentSequence.arp_style == 2)
                { // arp up & down
                    if (currentSequence.arp_step <= currentSequence.arp_length)
                    {
                        myNoteOnSeq(midiChannel, currentSequence.arp_note + arps[currentSequence.arp_chord][currentSequence.arp_step], currentSequence.chord_velocity);
                        currentSequence.arp_note_prev = currentSequence.arp_note + arps[currentSequence.arp_chord][currentSequence.arp_step];
                    }
                    else
                    {
                        myNoteOnSeq(midiChannel, currentSequence.arp_note + arps[currentSequence.arp_chord][currentSequence.arp_length * 2 - currentSequence.arp_step],
                                    currentSequence.chord_velocity);
                        currentSequence.arp_note_prev = currentSequence.arp_note + arps[currentSequence.arp_chord][currentSequence.arp_length * 2 - currentSequence.arp_step];
                    }
                }
                else if (currentSequence.arp_style == 3)
                {                     // arp random
                    uint8_t rnd1 = 3; // random(currentSequence.arp_length); TODO
                    myNoteOnSeq(midiChannel, currentSequence.arp_note + arps[currentSequence.arp_chord][rnd1 + currentSequence.element_shift] + (currentSequence.oct_shift * 12),
                                currentSequence.chord_velocity);
                    currentSequence.arp_note_prev = currentSequence.arp_note + arps[currentSequence.arp_chord][rnd1 + currentSequence.element_shift] + (currentSequence.oct_shift * 12);
                }
            }
        }
    }

    currentSequence.noteoffsent = false;

    currentSequence.arp_counter++;
    currentSequence.step++;
    if (currentSequence.arp_speed == 0) // Arp Speed 1/16
    {
        currentSequence.arp_step++;
    }
    else
    {
        // Arp Speed 1/8
        if (currentSequence.arp_counter > 1)
        {
            currentSequence.arp_counter = 0;
            currentSequence.arp_step++;
        }
    }

    if (currentSequence.arp_style != 2)
    {

        if ((currentSequence.arp_step > 1 && arps[currentSequence.arp_chord][currentSequence.arp_step] == 0) || currentSequence.arp_step == currentSequence.arp_length)
        {
            currentSequence.arp_step = 0;
        }
    }
    if (currentSequence.arp_style == 1 || currentSequence.arp_style == 2)
    {
        if (currentSequence.arp_length == 0)
            currentSequence.arp_length = 9;
    }
    if (currentSequence.arp_style == 2) // only for up&down
    {
        if ((currentSequence.arp_step > 1 && arps[currentSequence.arp_chord][currentSequence.arp_step] == 0) || currentSequence.arp_step == currentSequence.arp_length * 2)
        {
            currentSequence.arp_step = 0;
        }
    }
    if (currentSequence.step > 15)
    {
        currentSequence.step = 0;
        if (currentSequence.chain_length > 0)
        {
            currentSequence.chain_active_step++;
            if (currentSequence.chain_active_step > currentSequence.chain_length)
            {
                currentSequence.chain_active_step = 0;
            }
        }
    }
    interrupt_swapper = false;
}

void noteOffRoutine()
{
    seq_live_recording();

    if (currentSequence.noteoffsent == false)
    {
        if (currentSequence.prev_note > 0 && currentSequence.track_type > 0)
        {
            // Note not held
            if (currentSequence.Notes[currentSequence.patternchain[currentSequence.chain_active_step]][currentSequence.step] != 130)
            {
                myNoteOff(midiChannel, currentSequence.prev_note, 0);
                currentSequence.noteoffsent = true;
            }
            if (currentSequence.track_type == TrackType::CHORD)
            { // Chords
                if (currentSequence.prev_vel > 199)
                {
                    for (uint8_t x = currentSequence.element_shift; x < currentSequence.element_shift + currentSequence.chord_key_amount; x++) // play chord notes
                    {
                        myNoteOff(midiChannel, currentSequence.prev_note + arps[currentSequence.prev_vel - 200][x], 0);
                        currentSequence.noteoffsent = true;
                    }
                }
            }
            else if (currentSequence.track_type == TrackType::ARP)
            { // Arp
                myNoteOff(midiChannel, currentSequence.arp_note_prev, 0);
                currentSequence.noteoffsent = true;
            }
        }
    }
    interrupt_swapper = true;
}

// Runs in Interrupt Timer. Switches between the Noteon and Noteoff Task, each cycle
void sequencer()
{
    // interrupt_swapper = !interrupt_swapper;
    if (interrupt_swapper)
        noteOnRoutine();
    else
        noteOffRoutine();
}