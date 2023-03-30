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

https://github.com/wbajzek/arduino-arpeggiator/blob/master/arpeggiator.ino
*/

// Janelia Array available in libraries manager
#include <Array.h>
#include "Constants.h"
#include "Utils.h"
#include <ArduinoJson.h>
#include "Parameters.h"

#define SEQ_PATTERN_LEN 64

typedef enum ArpStyles
{
    UP,
    DOWN,
    BOUNCE,
    UPDOWN,
    RANDOMPLAY
} ArpStyles;

extern void myNoteOff(byte channel, byte note, byte velocity);
extern void noteOff(byte channel, byte note, byte velocity);
extern void noteOn(byte channel, byte note, byte velocity);

uint8_t currentSeqPosition = 0; // 1.1.1
uint8_t currentSeqNote = 60;    // C3
uint8_t seqCurrentOctPos = 3;   // C3

boolean arpRunning = false;

boolean arpUp = true;
uint8_t playBeat = 0;
uint8_t previousArpNote = 0;
uint8_t arpStyle = UP;
boolean arp_hold = false;
byte arpNotes[12] = {'\0'};
byte arpVels[12];
uint8_t arpNotesHeld = 0;
uint8_t arp_division = 1;
uint8_t arpRange = 3;
uint8_t bounceArpRange = 3;
int8_t currentArpOct = 0;
boolean upDownExtraNote = false;

const static char *ARP_STYLES[5] = {"Up", "Down", "Bounce", "Up Down", "Random"};
const static char *ARP_DIVISION_STR[10] = {"1/32", "1/24", "1/16", "1/12", "1/8", "1/6", "1/4", "1/3", "1/2", "1"};
const static uint8_t ARP_DIVISION_24PPQ[10] = {3, 4, 6, 8, 12, 16, 24, 32, 48, 96};
const static char *ONOFF[2] = {"Off", "On"};
const static char *ARP_RANGE_STR[7] = {"4 Down", "3 Down", "2 Down", "Base", "2 Up", "3 Up", "4 Up"};
const static int8_t ARP_RANGE[7] = {-3, -2, -1, 0, 1, 2, 3};

// empties out the arpeggio. used when switching modes, when in hold mode and
// a new arpeggio is started, or when the reset button is pushed.
FLASHMEM void resetNotes()
{
    playBeat = 0;
    currentArpOct = 0;
    bounceArpRange = arpRange;
    upDownExtraNote = false;
    if ((arpStyle == BOUNCE || arpStyle == UPDOWN) && ARP_RANGE[arpRange] < 0)
        arpUp = false;
    else
        arpUp = true;

    for (uint8_t i = 0; i < sizeof(arpNotes); i++)
        arpNotes[i] = '\0';
}

FLASHMEM uint8_t getNotesInArp()
{
    if (arpNotes[0] == '\0')
        return 0;
    uint8_t a = sizeof(arpNotes) - 1;
    while (arpNotes[a] == '\0')
    {
        a--;
    }
    return a + 1;
}

FLASHMEM void up()
{
    playBeat++;
    if (arpNotes[playBeat] == '\0')
    {
        playBeat = 0;
        if (ARP_RANGE[arpRange] > -1 && currentArpOct < ARP_RANGE[arpRange])
            currentArpOct++;
        else if (ARP_RANGE[arpRange] < 0 && currentArpOct > ARP_RANGE[arpRange])
            currentArpOct--;
        else
            currentArpOct = 0;
    }
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

        if (ARP_RANGE[arpRange] > -1 && currentArpOct < ARP_RANGE[arpRange])
            currentArpOct++;
        else if (ARP_RANGE[arpRange] < 0 && currentArpOct > ARP_RANGE[arpRange])
            currentArpOct--;
        else
            currentArpOct = 0;
    }
    else
        playBeat--;
}

FLASHMEM void upDown()
{
    if (ARP_RANGE[bounceArpRange] == 0 && getNotesInArp() == 1)
    {
        // Special case - one note, base range
        playBeat = 0;
    }
    else if (ARP_RANGE[bounceArpRange] > -1)
    {
        if (arpUp)
        {
            if (upDownExtraNote && currentArpOct == 0)
            {
                upDownExtraNote = false;
                return;
            }
            else
            {
                playBeat++;
            }
            if (arpNotes[playBeat] == '\0')
            {
                playBeat = 0;
                if (ARP_RANGE[bounceArpRange] != 0)
                    currentArpOct++;
            }
            if (playBeat == getNotesInArp() - 1 && currentArpOct == ARP_RANGE[bounceArpRange])
            {
                arpUp = false;
                upDownExtraNote = true;
            }
        }
        else
        {
            if (upDownExtraNote && currentArpOct == ARP_RANGE[bounceArpRange])
            {
                upDownExtraNote = false;
                return;
            }

            if (playBeat == 0)
            {
                playBeat = sizeof(arpNotes) - 1;
                while (arpNotes[playBeat] == '\0')
                {
                    playBeat--;
                }
                if (ARP_RANGE[bounceArpRange] != 0)
                    currentArpOct--;
            }
            else
            {
                playBeat--;
            }
            if (playBeat == 0 && currentArpOct == 0)
            {
                arpUp = true;
                upDownExtraNote = true;
            }
        }
    }
    else
    {
        if (arpUp)
        {
            if (upDownExtraNote && currentArpOct == ARP_RANGE[bounceArpRange])
            {
                upDownExtraNote = false;
                return;
            }
            else
            {
                playBeat++;
            }
            if (arpNotes[playBeat] == '\0')
            {
                playBeat = 0;
                currentArpOct++;
            }
            if (playBeat == getNotesInArp() - 1 && currentArpOct == 0)
            {
                arpUp = false;
                upDownExtraNote = true;
            }
        }
        else
        {
            if (upDownExtraNote && currentArpOct == 0)
            {
                upDownExtraNote = false;
                return;
            }
            if (playBeat == 0)
            {
                playBeat = sizeof(arpNotes) - 1;
                while (arpNotes[playBeat] == '\0')
                {
                    playBeat--;
                }
                currentArpOct--;
            }
            else
                playBeat--;
            if (playBeat == 0 && currentArpOct == ARP_RANGE[bounceArpRange])
            {
                arpUp = true;
                upDownExtraNote = true;
            }
        }
    }
}

FLASHMEM void bounce()
{

    if (ARP_RANGE[bounceArpRange] == 0 && getNotesInArp() == 1)
    {
        // Special case - one note, base range
        playBeat = 0;
    }
    else if (ARP_RANGE[bounceArpRange] > -1)
    {
        if (arpUp)
        {
            playBeat++;
            if (arpNotes[playBeat] == '\0')
            {
                playBeat = 0;
                if (ARP_RANGE[bounceArpRange] != 0)
                    currentArpOct++;
            }
            if (playBeat == getNotesInArp() - 1 && currentArpOct == ARP_RANGE[bounceArpRange])
                arpUp = false;
        }
        else
        {
            if (playBeat == 0)
            {
                playBeat = sizeof(arpNotes) - 1;
                while (arpNotes[playBeat] == '\0')
                {
                    playBeat--;
                }
                if (ARP_RANGE[bounceArpRange] != 0)
                    currentArpOct--;
            }
            else
                playBeat--;
            if (playBeat == 0 && currentArpOct == 0)
                arpUp = true;
        }
    }
    else
    {
        if (arpUp)
        {
            playBeat++;
            if (arpNotes[playBeat] == '\0')
            {
                playBeat = 0;
                currentArpOct++;
            }
            if (playBeat == getNotesInArp() - 1 && currentArpOct == 0)
                arpUp = false;
        }
        else
        {
            if (playBeat == 0)
            {
                playBeat = sizeof(arpNotes) - 1;
                while (arpNotes[playBeat] == '\0')
                {
                    playBeat--;
                }
                currentArpOct--;
            }
            else
                playBeat--;
            if (playBeat == 0 && currentArpOct == ARP_RANGE[bounceArpRange])
                arpUp = true;
        }
    }
}

FLASHMEM void randomPlay()
{
    if (arpNotes[1] == '\0')
        playBeat = 0;
    else
        playBeat = random(getNotesInArp());

    if (ARP_RANGE[arpRange] > -1)
        currentArpOct = random(ARP_RANGE[arpRange] + 1);
    else
        currentArpOct = -1 * random(-1 * (ARP_RANGE[arpRange] - 1));
}

typedef struct SequenceStruct
{
    uint32_t UID = 0;
    String SequenceName = F("-Empty-");
    float bpm = 120.0f;
    uint8_t length = SEQ_PATTERN_LEN;
    uint8_t Notes[SEQ_PATTERN_LEN] = {0};
    uint8_t Velocities[SEQ_PATTERN_LEN] = {0};
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
bool seqSwapper = false;

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
    if (arpRunning)
    {
        if ((arp_hold && getNotesInArp() > 0) || arpNotesHeld > 0)
        {
            if (arpNotes[playBeat] != '\0')
            {
                noteOn(midiChannel, arpNotes[playBeat] + (currentArpOct * 12), arpVels[playBeat]);
                previousArpNote = arpNotes[playBeat] + (currentArpOct * 12);
            }
            // Serial.printf("%d:%d %d  held:%d\n", currentArpOct, playBeat, arpUp, arpNotesHeld);
            //    decide what the next note is based on the mode.
            switch (arpStyle)
            {
            case DOWN:
                down();
                break;
            case BOUNCE:
                // Avoid problems when changing range
                if (bounceArpRange != arpRange && currentArpOct == 0 && playBeat == 0)
                    bounceArpRange = arpRange;
                bounce();
                break;
            case UPDOWN:
                // Avoid problems when changing range
                if (bounceArpRange != arpRange && currentArpOct == 0 && playBeat == 0)
                    bounceArpRange = arpRange;
                upDown();
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
            if (currentSequence.running)
            {
                // 130 signifies latched note
                if (currentSequence.Notes[currentSequence.step] != 130)
                {
                    noteOn(midiChannel, currentSequence.Notes[currentSequence.step], currentSequence.Velocities[currentSequence.step]);
                    currentSequence.prev_note = currentSequence.Notes[currentSequence.step];
                    currentSequence.prev_vel = currentSequence.Velocities[currentSequence.step];
                }
            }
        }

        currentSequence.noteoffsent = false;

        if (++currentSequence.step == currentSequence.length)
            currentSequence.step = 0;
    }
    seqSwapper = false;
}

FLASHMEM void noteOffRoutine()
{
    if (arpRunning)
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
            }
        }
    }
    seqSwapper = true;
}

// Runs in Interrupt Timer. Switches between the Noteon and Noteoff Task, each cycle
FLASHMEM void sequencer()
{
    if (seqSwapper)
        noteOnRoutine();
    else
        noteOffRoutine();
}