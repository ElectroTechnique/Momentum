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

https://github.com/wbajzek/arduino-arpeggiator/blob/master/arpeggiator.ino
*/

// Janelia Array available in libraries manager
#include <Array.h>
#include "Constants.h"
#include "Utils.h"
#include <ArduinoJson.h>
#include "Parameters.h"

#define SEQ_PATTERN_LEN 64

//  1 = Instrumenttrack, 2 = Chord, 3 = Arp
typedef enum TrackType
{
    INSTRUMENT = 1,
    CHORD = 2,
    ARP = 3
} TrackType;

typedef enum ArpSpeed
{
    SIXTEENTH,
    EIGHTH,
    QUARTER
} ArpSpeed;

typedef enum ArpStyle
{
    UP,
    DOWN,
    BOUNCE,
    UPDOWN,
    ONETHREE,
    ONETHREEEVEN,
    RANDOMPLAY
} ArpStyle;

extern void myNoteOff(byte channel, byte note, byte velocity);
extern void noteOff(byte channel, byte note, byte velocity);
extern void noteOn(byte channel, byte note, byte velocity);

uint8_t currentSeqPosition = 0; // 1.1.1
uint8_t currentSeqNote = 60;    // C3
uint8_t seqCurrentOctPos = 3;   // C3

boolean arpUp = false;
uint8_t playBeat = 0;
uint8_t previousArpNote = 0;
uint8_t arp_style = UP;
boolean arp_hold = false;
byte arpNotes[12] = {'\0'};
byte arpVels[12];
uint8_t arpNotesHeld = 0;
uint8_t arp_division = 1;
uint8_t arp_length = 8;

const static char *ARP_STYLES[7] = {"Up", "Down", "Bounce", "Up Down", "One Three", "One Three Even", "Random"};
const static char *ARP_DIVISION_STR[4] = {"1/32", "1/16", "1/8", "1/4"};
const static float ARP_DIVISION[4] = {0.5f, 1.0f, 2.0f, 4.0f};
const static char *ONOFF[2] = {"Off", "On"};

// empties out the arpeggio. used when switching modes, when in hold mode and
// a new arpeggio is started, or when the reset button is pushed.
FLASHMEM void resetNotes()
{
    for (uint8_t i = 0; i < sizeof(arpNotes); i++)
        arpNotes[i] = '\0';
}

FLASHMEM void up()
{
    playBeat++;
    if (arpNotes[playBeat] == '\0')
        playBeat = 0;
}

FLASHMEM void down()
{
    if (playBeat == 0)
    {
        playBeat = sizeof(arpNotes) - 1;
        while (arpNotes[playBeat] == '\0')
        {
            playBeat--;
        }
    }
    else
        playBeat--;
}

FLASHMEM void bounce()
{
    if (sizeof(arpNotes) == 1)
        playBeat = 0;
    else if (arpUp)
    {
        if (arpNotes[playBeat + 1] == '\0')
        {
            arpUp = false;
            playBeat--;
        }
        else
            playBeat++;
    }
    else
    {
        if (playBeat == 0)
        {
            arpUp = true;
            playBeat++;
        }
        else
            playBeat--;
    }
}

FLASHMEM void upDown()
{
    if (sizeof(arpNotes) == 1)
        playBeat = 0;
    else if (arpUp)
    {
        if (arpNotes[playBeat + 1] == '\0')
        {
            arpUp = false;
        }
        else
            playBeat++;
    }
    else
    {
        if (playBeat == 0)
        {
            arpUp = true;
        }
        else
            playBeat--;
    }
}

FLASHMEM void oneThree()
{
    if (arpUp)
        playBeat += 2;
    else
        playBeat--;

    arpUp = !arpUp;

    if (arpNotes[playBeat] == '\0')
    {
        playBeat = 0;
        arpUp = true;
    }
}

FLASHMEM void oneThreeEven()
{

    if (arpNotes[playBeat + 1] == '\0')
    {
        playBeat = 0;
        arpUp = true;
        return;
    }

    if (arpUp)
        playBeat += 2;
    else
        playBeat--;

    arpUp = !arpUp;
}

FLASHMEM void randomPlay()
{
    if (arpNotes[1] == '\0')
        playBeat = 0;
    else
        playBeat = random(arpNotesHeld);
}

const uint8_t chords[6][23] = {
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
    uint8_t Notes[SEQ_PATTERN_LEN] = {0};
    uint8_t Velocities[SEQ_PATTERN_LEN] = {0};
    uint8_t track_type = INSTRUMENT; //  1 = Instrumenttrack, 2 = Chord, 3 = Arp
    boolean noteoffsent = false;
    uint8_t step = 0;
    boolean running = false;
    boolean recording = false;

    uint8_t note_in;
    uint8_t note_in_velocity;
    int8_t transpose;

    uint8_t chord_velocity = 60;
    uint8_t chord_key_amount = 4;
    uint8_t element_shift = 0; // Not currently used
    int8_t oct_shift = 0;      // Not currently used

    uint32_t tempo_us = 125'000; // 60s/bpm

    uint8_t prev_note; // note_offs for every instrument track
    uint8_t prev_vel;
} SequenceStruct;

uint8_t currentSequenceIndex = 0;

boolean seqMidiSync = false;

SequenceStruct currentSequence;

String sequences[SEQUENCES_LIMIT];

FLASHMEM void concatSequenceFolderAndFilename(uint8_t filename, char *result)
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

    for (uint8_t j = 0; j < currentSequence.length; j++)
    {
        currentSequence.Notes[j] = Notes[j];
        currentSequence.Velocities[j] = Vel[j];
    }
    file.close();
    currentSeqPosition = 0;
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
    JsonArray Notes = doc.createNestedArray("Notes");
    JsonArray Vel = doc.createNestedArray("Vel");

    for (uint8_t j = 0; j < currentSequence.length; j++)
    {
        Notes.add(currentSequence.Notes[j]);
        Vel.add(currentSequence.Velocities[j]);
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

FLASHMEM void seq_live_recording()
{
    // record to sequencer if recording is active
    if (currentSequence.note_in > 0 && currentSequence.recording == true)
    {
        currentSequence.Notes[currentSequence.step] = currentSequence.note_in;
        currentSequence.Velocities[currentSequence.step] = currentSequence.note_in_velocity;

        currentSequence.note_in = 0;
        currentSequence.note_in_velocity = 0;
    }
}

FLASHMEM void noteOnRoutine()
{
    if (currentSequence.track_type == ARP)
    {
        if (arp_hold || arpNotesHeld > 0)
        {
            if (arpNotes[playBeat] != '\0')
                // play the current note
                noteOn(midiChannel, arpNotes[playBeat], arpVels[playBeat]);

            previousArpNote = arpNotes[playBeat];

            // decide what the next note is based on the mode.
            switch (arp_style)
            {
            case DOWN:
                down();
                break;
            case BOUNCE:
                bounce();
                break;
            case UPDOWN:
                upDown();
                break;
            case ONETHREE:
                oneThree();
                break;
            case ONETHREEEVEN:
                oneThreeEven();
                break;
            case RANDOMPLAY:
                randomPlay();
                break;
            case UP:
            default:
                up();
                break;
            }
        }
    }
    else
    {

        if (currentSequence.note_in > 0 && currentSequence.note_in < 62 && currentSequence.recording == false)
        {
            myNoteOff(midiChannel, currentSequence.Notes[currentSequence.step] + currentSequence.transpose, 0);
            myNoteOff(midiChannel, currentSequence.Notes[currentSequence.step - 1] + currentSequence.transpose, 0);
            if (currentSequence.note_in > 65)
                currentSequence.note_in = currentSequence.note_in - 12;
            currentSequence.transpose = currentSequence.note_in % 12;
            currentSequence.transpose = currentSequence.transpose - 12;
            currentSequence.note_in = 0;
        }

        seq_live_recording();

        if (currentSequence.Notes[currentSequence.step] > 0)
        {
            if (currentSequence.track_type == TrackType::INSTRUMENT)
            {
                // 130 signifies latched note
                if (currentSequence.Notes[currentSequence.step] != 130)
                {
                    noteOn(midiChannel, currentSequence.Notes[currentSequence.step], currentSequence.Velocities[currentSequence.step]);
                    currentSequence.prev_note = currentSequence.Notes[currentSequence.step];
                    currentSequence.prev_vel = currentSequence.Velocities[currentSequence.step];
                }
            }
            else if (currentSequence.track_type == TrackType::CHORD) // Chords
            {
                if (currentSequence.Velocities[currentSequence.step] > 199) // Signifies range?
                {

                    // myNoteOnSeq(midiChannel, currentSequence.Notes[currentSequence.step], currentSequence.chord_velocity);  // basenote

                    for (uint8_t x = currentSequence.element_shift; x < currentSequence.element_shift + currentSequence.chord_key_amount; x++) // play chord notes
                    {
                        noteOn(midiChannel, currentSequence.Notes[currentSequence.step] + (currentSequence.oct_shift * 12) + chords[currentSequence.Velocities[currentSequence.step] - 200][x], currentSequence.chord_velocity);
                    }
                    currentSequence.prev_note = currentSequence.Notes[currentSequence.step] + (currentSequence.oct_shift * 12);
                    currentSequence.prev_vel = currentSequence.Velocities[currentSequence.step];
                }
            }
        }

        currentSequence.noteoffsent = false;

        if (++currentSequence.step == currentSequence.length)
            currentSequence.step = 0;
    }
    interrupt_swapper = false;
}

FLASHMEM void noteOffRoutine()
{
    if (currentSequence.track_type == TrackType::ARP && currentSequence.running)
    {
        // Turn off previous note
        noteOff(midiChannel, previousArpNote, 0);
    }
    else
    {
        seq_live_recording();

        if (currentSequence.noteoffsent == false)
        {
            if (currentSequence.prev_note > 0)
            {
                // Note not held
                if (currentSequence.Notes[currentSequence.step] != 130) // Latched note
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
                            myNoteOff(midiChannel, currentSequence.prev_note + chords[currentSequence.prev_vel - 200][x], 0);
                            currentSequence.noteoffsent = true;
                        }
                    }
                }
            }
        }
    }
    interrupt_swapper = true;
}

// Runs in Interrupt Timer. Switches between the Noteon and Noteoff Task, each cycle
FLASHMEM void sequencer()
{
    if (interrupt_swapper)
        noteOnRoutine();
    else
        noteOffRoutine();
}