// Janelia Array available in libraries manager
#include <Array.h>
#include "Constants.h"
#include "Utils.h"
#include <ArduinoJson.h>

struct SequenceUIDAndName
{
    uint32_t SequenceUID;
    String SequenceName;
};

SequenceStruct currentSequence;

typedef Array<SequenceUIDAndName, SEQUENCES_LIMIT> Sequences;
Sequences sequences;

FLASHMEM void concatSequenceFolderAndUID(uint32_t filename, char *result)
{
    char buf[20];
    strcpy(result, SEQUENCE_FOLDER_NAME_SLASH); // copy string one into the result.
    strcat(result, utoa(filename, buf, 10));    // append string two to the result
}

// Filename is UID
FLASHMEM void loadSequence(uint32_t filename)
{
    // Open file for reading - UID is Filename
    char result[30];
    concatSequenceFolderAndUID(filename, result);
    File file = SD.open(result);

    // Allocate a temporary JsonDocument
    // Don't forget to change the capacity to match your requirements.
    // Use https://arduinojson.org/v6/assistant to compute the capacity.
    StaticJsonDocument<2048> doc;

    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, file);
    if (error)
    {
        Serial.print(F("loadSequence() - Failed to read file:"));
        Serial.println(filename);
        return;
    }
    // Copy values from the JsonDocument to the SequenceStruct
    strncpy(currentSequence.SequenceName, doc["SequenceName"], sizeof(currentSequence.SequenceName));
    currentSequence.UID = doc["UID"];

    file.close();
}

FLASHMEM void saveSequence()
{
    StaticJsonDocument<1024> doc;
    // Need to generate a new UID as the sequence settings may have changed if overwriting an existing sequence
    String output;
    serializeJson(doc, output);      // Generate JSON without UID
    uint32_t iUID = getHash(output); // Generate UID
    currentSequence.UID = iUID;      // Give current patch a UID
    doc["UID"] = iUID;               // Insert UID
    serializeJson(doc, output);      // Generate complete JSON to save

    char result[30];
    concatSequenceFolderAndUID(iUID, result);
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