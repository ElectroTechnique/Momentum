// Janelia Array available in libraries manager
#include <Array.h>
#include "Constants.h"
#include <ArduinoJson.h>
#include <RokkitHash.h>

#define TOTALCHARS 64

const static char *BANKS_FILE_NAME = "Banknames";
const static char *BANK_FOLDER_NAMES[] = {"Bank1", "Bank2", "Bank3", "Bank4", "Bank5", "Bank6", "Bank7", "Bank8"};
const static char *BANK_FOLDER_NAMES_SLASH[] = {"Bank1/", "Bank2/", "Bank3/", "Bank4/", "Bank5/", "Bank6/", "Bank7/", "Bank8/"};
const static char *SEQUENCE_FOLDER_NAME = "Sequences";
const static char *SEQUENCE_FOLDER_NAME_SLASH = "Sequences/";

const static char CHARACTERS[TOTALCHARS] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', ' ', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', ' ', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0'};
int charIndex = 0;
char currentCharacter = 0;

uint8_t currentBank = 0;
uint8_t currentPatchIndex = 0;

PatchStruct currentPatch;

struct PatchUIDAndName
{
  uint32_t patchUID;
  String patchName;
};

typedef Array<PatchUIDAndName, PATCHES_LIMIT> Patches;

Patches patches;

// Computes a hash of the parameter values to create a UID for each patch that is stored with it.
// This can also be used to identify identical patches. Hash takes about 2.8us on TeensyMM
FLASHMEM uint32_t getHash(String tohash)
{
  return rokkit(tohash.c_str(), strlen(tohash.c_str()));
}

FLASHMEM void savePatch()
{
  StaticJsonDocument<1024> doc;

  doc["PatchName"] = currentPatch.PatchName;

  JsonObject OSC1 = doc.createNestedObject("OSC1");
  OSC1["Pitch"] = currentPatch.PitchA;
  OSC1["Waveform"] = currentPatch.WaveformA;
  OSC1["Level"] = currentPatch.OscLevelA;
  OSC1["PWAmount"] = currentPatch.PWA_Amount;
  OSC1["PWMAmount"] = currentPatch.PWMA_Amount;

  JsonObject OSC2 = doc.createNestedObject("OSC2");
  OSC2["Pitch"] = currentPatch.PitchB;
  OSC2["Waveform"] = currentPatch.WaveformB;
  OSC2["Level"] = currentPatch.OscLevelB;
  OSC2["PWAmount"] = currentPatch.PWB_Amount;
  OSC2["PWMAmount"] = currentPatch.PWMB_Amount;

  doc["PWMSource"] = currentPatch.PWMSource;
  doc["PWMRate"] = currentPatch.PWMRate;
  doc["Detune"] = currentPatch.Detune;
  doc["NoiseLevel"] = currentPatch.NoiseLevel;
  doc["Unison"] = currentPatch.Unison;
  doc["OscFX"] = currentPatch.OscFX;
  doc["PitchEnv"] = currentPatch.PitchEnv;
  doc["PitchLFOAmt"] = currentPatch.PitchLFOAmt;
  doc["PitchLFORate"] = currentPatch.PitchLFORate;
  doc["PitchLFOWaveform"] = currentPatch.PitchLFOWaveform;
  doc["PitchLFORetrig"] = currentPatch.PitchLFORetrig;
  doc["PitchLFOMidiClkSync"] = currentPatch.PitchLFOMidiClkSync;

  JsonObject Filter = doc.createNestedObject("Filter");
  Filter["Freq"] = currentPatch.FilterFreq;
  Filter["Res"] = currentPatch.FilterRes;
  Filter["Mixer"] = currentPatch.FilterMixer;
  Filter["FilterEnv"] = currentPatch.FilterEnv;
  Filter["LFORate"] = currentPatch.FilterLFORate;
  Filter["LFORetrig"] = currentPatch.FilterLFORetrig;
  Filter["LFOMidiClkSync"] = currentPatch.FilterLFOMidiClkSync;
  Filter["LfoAmt"] = currentPatch.FilterLfoAmt;
  Filter["LFOWaveform"] = currentPatch.FilterLFOWaveform;
  Filter["Attack"] = currentPatch.FilterAttack;
  Filter["Decay"] = currentPatch.FilterDecay;
  Filter["Sustain"] = currentPatch.FilterSustain;
  Filter["Release"] = currentPatch.FilterRelease;

  JsonObject Amp = doc.createNestedObject("Amp");
  Amp["Attack"] = currentPatch.Attack;
  Amp["Decay"] = currentPatch.Decay;
  Amp["Sustain"] = currentPatch.Sustain;
  Amp["Release"] = currentPatch.Release;

  doc["KeyTracking"] = currentPatch.KeyTracking;
  doc["LFOTempoValue"] = currentPatch.LFOTempoValue;
  doc["LFOSyncFreq"] = currentPatch.LFOSyncFreq;
  doc["MidiClkTimeInterval"] = currentPatch.MidiClkTimeInterval;
  doc["VelocitySensitivity"] = currentPatch.VelocitySensitivity;
  doc["ChordDetune"] = currentPatch.ChordDetune;
  doc["MonophonicMode"] = currentPatch.MonophonicMode;
  doc["Glide"] = currentPatch.Glide;
  doc["EffectAmt"] = currentPatch.EffectAmt;
  doc["EffectMix"] = currentPatch.EffectMix;

  // Need to generate a new UID as the patch settings may have changed if overwriting an existing patch
  String output;
  serializeJson(doc, output);      // Generate JSON without UID
  uint32_t iUID = getHash(output); // Generate UID
  currentPatch.UID = iUID;         // Give current patch a UID
  doc["UID"] = iUID;               // Insert UID
  serializeJson(doc, output);      // Generate complete JSON to save

  File file = SD.open(String(iUID).c_str(), FILE_WRITE);
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

FLASHMEM void checkSDCardStructure()
{
  // Expecting 8 Bank folders,Sequence folder and Bankname file
  for (uint8_t i; i < 8; i++)
  {
    if (!SD.exists(BANK_FOLDER_NAMES_SLASH[i]))
    {
      SD.mkdir(BANK_FOLDER_NAMES[i]);
      // Save default patch file into Bank 1 folder
      if (i == 0)
        savePatch();
    }
  }
  if (!SD.exists(SEQUENCE_FOLDER_NAME_SLASH))
  {
    SD.mkdir(SEQUENCE_FOLDER_NAME);
  }
  if (!SD.exists(BANKS_FILE_NAME))
  {
    SD.mkdir(BANKS_FILE_NAME);
  }
}

// Filename is UID
FLASHMEM void loadPatch(uint8_t bank, uint32_t filename)
{
  // Open file for reading - UID is Filename
  char buf[12];
  char result[30]; // array to hold the result.

  strcpy(result, *BANK_FOLDER_NAMES_SLASH); // copy string one into the result.
  strcat(result, utoa(filename, buf, 10));  // append string two to the result
  File file = SD.open(result);

  // Allocate a temporary JsonDocument
  // Don't forget to change the capacity to match your requirements.
  // Use https://arduinojson.org/v6/assistant to compute the capacity.
  StaticJsonDocument<2048> doc;

  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, file);
  if (error)
  {
    Serial.print(F("loadPatch() - Failed to read file:"));
    Serial.println(filename);
    return;
  }
  // Copy values from the JsonDocument to the PatchStruct
  strncpy(currentPatch.PatchName, doc["PatchName"], sizeof(currentPatch.PatchName));
  currentPatch.UID = doc["UID"];

  JsonObject OSC1 = doc["OSC1"];
  currentPatch.PitchA = OSC1["Pitch"];
  currentPatch.WaveformA = OSC1["Waveform"];
  currentPatch.OscLevelA = OSC1["Level"];
  currentPatch.PWA_Amount = OSC1["PWAmount"];
  currentPatch.PWMA_Amount = OSC1["PWMAmount"];

  JsonObject OSC2 = doc["OSC2"];
  currentPatch.PitchB = OSC2["Pitch"];
  currentPatch.WaveformB = OSC2["Waveform"];
  currentPatch.OscLevelB = OSC2["Level"];
  currentPatch.PWB_Amount = OSC2["PWAmount"];
  currentPatch.PWMB_Amount = OSC2["PWMAmount"];

  currentPatch.PWMSource = doc["PWMSource"];
  currentPatch.PWMRate = doc["PWMRate"];
  currentPatch.Detune = doc["Detune"];
  currentPatch.NoiseLevel = doc["NoiseLevel"];
  currentPatch.Unison = doc["Unison"];
  currentPatch.OscFX = doc["OscFX"];
  currentPatch.PitchEnv = doc["PitchEnv"];
  currentPatch.PitchLFOAmt = doc["PitchLFOAmt"];
  currentPatch.PitchLFORate = doc["PitchLFORate"];
  currentPatch.PitchLFOWaveform = doc["PitchLFOWaveform"];
  currentPatch.PitchLFORetrig = doc["PitchLFORetrig"];
  currentPatch.PitchLFOMidiClkSync = doc["PitchLFOMidiClkSync"];

  JsonObject Filter = doc["Filter"];
  currentPatch.FilterFreq = Filter["Freq"];
  currentPatch.FilterRes = Filter["Res"];
  currentPatch.FilterMixer = Filter["Mixer"];
  currentPatch.FilterEnv = Filter["Env"];
  currentPatch.FilterLFORate = Filter["LFORate"];
  currentPatch.FilterLFORetrig = Filter["LFORetrig"];
  currentPatch.FilterLFOMidiClkSync = Filter["LFOMidiClkSync"];
  currentPatch.FilterLfoAmt = Filter["LfoAmt"];
  currentPatch.FilterLFOWaveform = Filter["LFOWaveform"];
  currentPatch.FilterAttack = Filter["Attack"];
  currentPatch.FilterDecay = Filter["Decay"];
  currentPatch.FilterSustain = Filter["Sustain"];
  currentPatch.FilterRelease = Filter["Release"];

  JsonObject Amp = doc["Amp"];
  currentPatch.Attack = Amp["Attack"];
  currentPatch.Decay = Amp["Decay"];
  currentPatch.Sustain = Amp["Sustain"];
  currentPatch.Release = Amp["Release"];

  currentPatch.KeyTracking = doc["KeyTracking"];
  currentPatch.LFOTempoValue = doc["LFOTempoValue"];
  currentPatch.LFOSyncFreq = doc["LFOSyncFreq"];
  currentPatch.MidiClkTimeInterval = doc["MidiClkTimeInterval"];
  currentPatch.VelocitySensitivity = doc["VelocitySensitivity"];
  currentPatch.ChordDetune = doc["ChordDetune"];
  currentPatch.MonophonicMode = doc["MonophonicMode"];
  currentPatch.Glide = doc["Glide"];
  currentPatch.EffectAmt = doc["EffectAmt"];
  currentPatch.EffectMix = doc["EffectMix"];

  file.close();
}

// Patchname from a files and return it
char *getPatchName(File file)
{
  // Allocate a temporary JsonDocument
  // Don't forget to change the capacity to match your requirements.
  // Use https://arduinojson.org/v6/assistant to compute the capacity.
  StaticJsonDocument<2048> doc;

  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, file);
  if (error)
  {
    Serial.println(F("getPatchName() - Failed to read file"));
    return 0;
  }
  // Copy values from the JsonDocument to the PatchStruct
  return doc["PatchName"];
}

void readPatchFile(File patchFile)
{
  uint32_t uid = strtoul(patchFile.name(), NULL, 0);
  patches.push_back(PatchUIDAndName{uid, getPatchName(patchFile)});
  Serial.println(String(patches.back().patchUID) + ":" + patches.back().patchName);
}

// Loads Patchnames into circular buffer for display only and uses UID (filename) to recall
FLASHMEM void loadPatchNames()
{
  Serial.println("LoadPatchNames()");
  patches.clear();
  for (uint8_t i; i < BANKS_LIMIT; i++)
  {
    File bankDir = SD.open(BANK_FOLDER_NAMES_SLASH[i]);
    File patchFile;
    while (patchFile = bankDir.openNextFile())
    {
      if (patches.full())
      {
        Serial.println("More than max number of patches is being loaded into bank" + String(i + 1));
        patchFile.close();
        bankDir.close();
        break;
      }
      else
      {
        readPatchFile(patchFile);
      }
      patchFile.close();
    }
  }
}

uint8_t incCurrentPatchIndex()
{
  if (currentPatchIndex < PATCHES_LIMIT - 1)
  {
    return ++currentPatchIndex;
  }
  else
  {
    // Go back to Start from front of array
    currentPatchIndex = 0;
    return currentPatchIndex;
  }
}

uint8_t decCurrentPatchIndex()
{
  if (currentPatchIndex > 0)
  {
    return --currentPatchIndex;
  }
  else
  {
    // Go to back of array
    currentPatchIndex = patches.size() - 1;
    return currentPatchIndex;
  }
}

// FLASHMEM void setPatchesOrdering(int no)
// {
//   if (patches.size() < 2)
//     return;
//   while (patches.front().patchNo != no)
//   {
//     patches.push_back(patches.shift());
//   }
// }