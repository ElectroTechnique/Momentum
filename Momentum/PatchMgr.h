// Janelia Array available in libraries manager
/*

TESTS
Save new patch without naming / cancel
Save new patch with naming / cancel

Save over existing patch without renaming / cancel
Save over existing patch with renaming / cancel

Delete patch from first, middle and end

Rename bank / cancel
Delete bank contents / cancel


*/

#include <Array.h>
#include "Constants.h"
#include "Utils.h"
#include <ArduinoJson.h>

uint8_t tempBankIndex = 0;
uint8_t currentBankIndex = 0;
uint8_t currentPatchIndex = 0;
uint8_t previousBankIndex = 0;
uint8_t previousPatchIndex = 0;

PatchStruct currentPatch;

struct PatchUIDAndName
{
  uint32_t patchUID;
  String patchName;
};

typedef Array<PatchUIDAndName, PATCHES_LIMIT> Patches;

Patches patches;

FLASHMEM void concatBankAndUID(uint8_t bank, uint32_t filename, char *result)
{
  char buf[12];
  strcpy(result, BANK_FOLDER_NAMES_SLASH[bank]); // copy string one into the result.
  strcat(result, utoa(filename, buf, 10));       // append string two to the result
}

FLASHMEM void concatBankAndFilename(uint8_t bank, const char *filename, char *result)
{
  // char buf[12];
  strcpy(result, BANK_FOLDER_NAMES_SLASH[bank]); // copy string one into the result.
  strcat(result, filename);                      // append string two to the result
}

FLASHMEM void createPatchIndexFile(uint8_t bankIndex)
{
  char result[30];
  concatBankAndFilename(bankIndex, PATCH_INDEX_FILE_NAME, result);
  SD.remove(result);
  File file = SD.open(result, FILE_WRITE);
  if (!file)
  {
    Serial.println(F("Failed to create patch index file"));
    return;
  }
  StaticJsonDocument<1024> doc;
  for (uint8_t i = 0; i < patches.size(); i++)
  {
    doc[String(i + 1)] = patches[i].patchUID;
  }
  serializeJson(doc, file);
  file.close();
}

FLASHMEM void insertIntoPatchIndexFile(uint8_t bankIndex, uint8_t index, uint32_t UID)
{
  char result[30];
  concatBankAndFilename(bankIndex, PATCH_INDEX_FILE_NAME, result);
  DynamicJsonDocument doc(1024);

  File file = SD.open(result);
  if (!file)
  {
    Serial.println(F("No existing patch index file"));
  }
  else
  {
    deserializeJson(doc, file);
    serializeJsonPretty(doc, Serial);
  }
  file.close();
  file = SD.open(result, FILE_WRITE);
  file.seek(0);
  file.print("");
  doc[String(index + 1)] = UID;
  serializeJsonPretty(doc, Serial);
  serializeJson(doc, file);
  file.close();
}

FLASHMEM Patches getPatchesCopy()
{
  Patches p;
  for (uint8_t i = 0; i < patches.size(); i++)
  {
    p.push_back(PatchUIDAndName{patches[i].patchUID, patches[i].patchName});
  }
  return p;
}

FLASHMEM void sortPatchesByIndex(uint8_t bankIndex)
{
}

// Patchname from a files and return it
FLASHMEM char *getPatchName(File file)
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

// Loads Patchnames into array for display only and uses UID (filename) to recall
FLASHMEM void loadPatchNamesFromBank(uint8_t bankIndex)
{
  Serial.println(F("LoadPatchNames from Bank") + String(bankIndex + 1));
  patches.clear();
  File bankDir = SD.open(BANK_FOLDER_NAMES_SLASH[bankIndex]);
  File patchFile;
  while (patchFile = bankDir.openNextFile())
  {
    if (!patchFile)
      break;
    if (strcmp(patchFile.name(), PATCH_INDEX_FILE_NAME) == 0)
      continue;
    if (patches.full())
    {
      Serial.println(F("More than max number of patches is being loaded into bank") + String(bankIndex + 1));
      patchFile.close();
      bankDir.close();
      break;
    }
    else
    {
      uint32_t uid = strtoul(patchFile.name(), NULL, 0);
      patches.push_back(PatchUIDAndName{uid, getPatchName(patchFile)});
    }
    patchFile.close();
  }

  if (patches.empty())
  {
    // Dummy patch
    patches.push_back(PatchUIDAndName{0, "-Empty-"});
  }
  else
  {
    // Sort
    char result[30];
    concatBankAndFilename(bankIndex, PATCH_INDEX_FILE_NAME, result);
    File file = SD.open(result);
    if (file)
    {
      StaticJsonDocument<1024> doc;
      DeserializationError error = deserializeJson(doc, file);
      // file.close();
      if (error)
      {
        Serial.print(F("sortPatchesByIndex() Deserialise error:"));
        Serial.println(error.c_str());
        file.close();
        return;
      }
      JsonObject obj = doc.as<JsonObject>();
      Patches patchesTemp = getPatchesCopy();
      patches.clear();
      for (JsonPair p : obj)
      {
        for (uint8_t i = 0; i < patchesTemp.size(); i++)
        {
          // Assume keys are in ascending order
          if (patchesTemp[i].patchUID == p.value())
          {
            patches.push_back(PatchUIDAndName{patchesTemp[i].patchUID, patchesTemp[i].patchName});
            break;
          }
        }
      }
      file.close();
    }
  }
}

FLASHMEM boolean savePatch(uint8_t bankIndex, uint8_t index)
{
  StaticJsonDocument<1024> doc;

  doc["PatchName"] = currentPatch.PatchName;

  JsonObject OSC1 = doc.createNestedObject("OSC1");
  OSC1["Pitch"] = currentPatch.PitchA;
  OSC1["Waveform"] = currentPatch.WaveformA;
  OSC1["Level"] = currentPatch.OscLevelA;
  OSC1["PWAmount"] = currentPatch.PWA_Amount;
  OSC1["PWMAmount"] = currentPatch.PWMA_Amount;
  OSC1["PWMSource"] = currentPatch.PWMSourceA;
  OSC1["PWMRate"] = currentPatch.PWMRateA;

  JsonObject OSC2 = doc.createNestedObject("OSC2");
  OSC2["Pitch"] = currentPatch.PitchB;
  OSC2["Waveform"] = currentPatch.WaveformB;
  OSC2["Level"] = currentPatch.OscLevelB;
  OSC2["PWAmount"] = currentPatch.PWB_Amount;
  OSC2["PWMAmount"] = currentPatch.PWMB_Amount;
  OSC2["PWMSource"] = currentPatch.PWMSourceB;
  OSC2["PWMRate"] = currentPatch.PWMRateB;

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
  doc["FilterEnvShape"] = currentPatch.FilterEnvShape;
  doc["AmpEnvShape"] = currentPatch.AmpEnvShape;
  doc["GlideShape"] = currentPatch.GlideShape;
  doc["PitchBend"] = currentPatch.PitchBend;
  doc["ModWheelDepth"] = currentPatch.ModWheelDepth;

  // Need to generate a new UID as the patch settings may have changed if overwriting an existing patch
  String output;
  serializeJson(doc, output);      // Generate JSON without UID
  uint32_t iUID = getHash(output); // Generate UID
  currentPatch.UID = iUID;         // Give current patch a UID
  doc["UID"] = iUID;               // Insert UID
  serializeJson(doc, output);      // Generate complete JSON to save

  char result[30];
  concatBankAndUID(bankIndex, iUID, result);
  File file = SD.open(result, FILE_WRITE);
  if (!file)
  {
    Serial.println(F("Failed to create file:") + String(result));
    file.close();
    return false;
  }
  if (serializeJson(doc, file) == 0)
  {
    Serial.println(F("savePatch() - Failed to write to file"));
    file.close();
    return false;
  }
  file.close();
  // Save index number to Patchindex
  // createPatchIndexFile(bankIndex);
  insertIntoPatchIndexFile(bankIndex, index, iUID);
  loadPatchNamesFromBank(bankIndex);
  Serial.print(F("Saved patch:"));
  Serial.println(iUID);
  return true;
}

FLASHMEM void saveBankName(char *bankname)
{
  DynamicJsonDocument doc(1024);

  File file = SD.open(BANKS_FILE_NAME);
  if (!file)
  {
    Serial.println(F("Banks file missing"));
  }
  else
  {
    deserializeJson(doc, file);
    serializeJsonPretty(doc, Serial);
  }
  file.close();
  file = SD.open(BANKS_FILE_NAME, FILE_WRITE);
  file.seek(0);
  file.print("");
  doc[String(BANK_FOLDER_NAMES[tempBankIndex])] = String(bankname);
  serializeJsonPretty(doc, Serial);
  serializeJson(doc, file);
  file.close();
}

FLASHMEM void checkSDCardStructure()
{
  // Expecting 8 Bank folders,Sequence folder and Bankname file
  for (uint8_t i = 0; i < 8; i++)
  {
    if (!SD.exists(BANK_FOLDER_NAMES_SLASH[i]))
    {
      SD.mkdir(BANK_FOLDER_NAMES[i]);
      Serial.println(F("Creating Bank dir:") + String(BANK_FOLDER_NAMES[i]));
      // Save default patch file into Bank 1 folder at first index
      if (i == 0)
        savePatch(0, 0);
    }
  }
  if (!SD.exists(SEQUENCE_FOLDER_NAME_SLASH))
  {
    SD.mkdir(SEQUENCE_FOLDER_NAME);
  }
  if (!SD.exists(PERFORMANCE_FOLDER_NAME_SLASH))
  {
    SD.mkdir(PERFORMANCE_FOLDER_NAME);
  }
  if (!SD.exists(BANKS_FILE_NAME))
  {
    File banksfile = SD.open(BANKS_FILE_NAME, FILE_WRITE);
    if (banksfile)
    {
      banksfile.println(BANKS_FILE_CONTENTS);
      banksfile.close();
    }
  }
}

FLASHMEM void loadBankNames()
{
  File file = SD.open("Banknames");
  if (!file)
  {
    Serial.println(F("Failed to read banknames file"));
  }

  StaticJsonDocument<512> doc;

  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, file);
  if (error)
  {
    Serial.println(F("Failed to deserialise banknames file"));
    return;
  }
  // Copy values from the JsonDocument to the banknames array
  for (u_int8_t i = 0; i < 8; i++)
  {
    strcpy(bankNames[i], doc[BANK_FOLDER_NAMES[i]]);
  }
  file.close();
}

// Filename is UID
FLASHMEM void loadPatch(uint8_t bank, uint32_t filename)
{
  // Open file for reading - UID is Filename
  char result[30];
  concatBankAndUID(bank, filename, result);
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
  currentPatch.PWMSourceA = OSC1["PWMSource"];
  currentPatch.PWMRateA = OSC1["PWMRate"];

  JsonObject OSC2 = doc["OSC2"];
  currentPatch.PitchB = OSC2["Pitch"];
  currentPatch.WaveformB = OSC2["Waveform"];
  currentPatch.OscLevelB = OSC2["Level"];
  currentPatch.PWB_Amount = OSC2["PWAmount"];
  currentPatch.PWMB_Amount = OSC2["PWMAmount"];
  currentPatch.PWMSourceB = OSC2["PWMSource"];
  currentPatch.PWMRateB = OSC2["PWMRate"];

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
  currentPatch.FilterEnvShape = doc["FilterEnvShape"];
  currentPatch.AmpEnvShape = doc["AmpEnvShape"];
  currentPatch.GlideShape = doc["GlideShape"];
  currentPatch.PitchBend = doc["PitchBend"];
  currentPatch.ModWheelDepth = doc["ModWheelDepth"];

  file.close();
}

FLASHMEM void reinitialisePatch()
{
  Serial.println(F("Reinitialise Patch"));
  PatchStruct ps;
  currentPatch = ps;
}

FLASHMEM int8_t findPatchIndex(uint32_t UID)
{
  for (uint8_t i = 0; i < patches.size(); i++)
  {
    Serial.printf("findPatchIndex()  UID:%u currentUID:%u index:%i \n", UID, patches[i].patchUID, i);
    if (UID == patches[i].patchUID)
    {
      return i;
    }
  }
  // Could be that patch array isn't for this bank, but patches
  // can be deleted from SD card from other bank folders
  Serial.println(F("UID not found in patch array:") + String(UID));
  return -1;
}

FLASHMEM void deletePatch(uint8_t bank, uint32_t UID)
{
  // delete from SD card
  char result[30];
  concatBankAndUID(bank, UID, result);
  if (!SD.remove(result))
  {
    Serial.println(F("Couldn't delete from SD card"));
  }
  // Remove from array if patch is in current bank
  if (bank == currentBankIndex)
  {
    int8_t ind = findPatchIndex(UID);
    if (ind > -1)
    {
      patches.remove(ind);
      Serial.printf("Deleted Patch:%s UID:%u index:%i \n", result, UID, ind);
    }
  }
}

FLASHMEM void deleteBank(uint8_t bank)
{
  for (uint8_t i = 0; i < patches.size(); i++)
  {
    deletePatch(bank, patches[i].patchUID);
  }
}

FLASHMEM uint8_t incCurrentPatchIndex()
{
  if (currentPatchIndex < patches.size() - 1)
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

FLASHMEM uint8_t decCurrentPatchIndex()
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

FLASHMEM uint8_t incCurrentBankIndex()
{
  if (currentBankIndex < 7)
  {
    return ++currentBankIndex;
  }
  else
  {
    // Go back to Start from front of array
    currentBankIndex = 0;
    return currentBankIndex;
  }
}

FLASHMEM uint8_t decCurrentBankIndex()
{
  if (currentBankIndex > 0)
  {
    return --currentBankIndex;
  }
  else
  {
    // Go to back of array
    currentBankIndex = 7;
    return currentBankIndex;
  }
}

FLASHMEM uint8_t incTempBankIndex()
{
  if (tempBankIndex < 7)
  {
    return ++tempBankIndex;
  }
  else
  {
    // Go back to Start from front of array
    tempBankIndex = 0;
    return tempBankIndex;
  }
}

FLASHMEM uint8_t decTempBankIndex()
{
  if (tempBankIndex > 0)
  {
    return --tempBankIndex;
  }
  else
  {
    // Go to back of array
    tempBankIndex = 7;
    return tempBankIndex;
  }
}