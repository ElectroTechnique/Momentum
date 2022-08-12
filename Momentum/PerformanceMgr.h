// Janelia Array available in libraries manager
#include <Array.h>
#include "Constants.h"
#include "Utils.h"
#include <ArduinoJson.h>

struct PerformanceUIDAndName
{
    uint32_t performanceUID;
    String performanceName;
};

PerformanceStruct currentPerformance;

typedef Array<PerformanceUIDAndName, PERFORMANCES_LIMIT> Performances;
Performances performances;

FLASHMEM void concatPerformanceFolderAndUID(uint32_t filename, char *result)
{
    char buf[20];
    strcpy(result, PERFORMANCE_FOLDER_NAME_SLASH); // copy string one into the result.
    strcat(result, utoa(filename, buf, 10));       // append string two to the result
}

// Filename is UID
FLASHMEM void loadPerformance(uint32_t filename)
{
    // Open file for reading - UID is Filename
    char result[30];
    concatPerformanceFolderAndUID(filename, result);
    File file = SD.open(result);

    // Allocate a temporary JsonDocument
    // Don't forget to change the capacity to match your requirements.
    // Use https://arduinojson.org/v6/assistant to compute the capacity.
    StaticJsonDocument<2048> doc;

    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, file);
    if (error)
    {
        Serial.print(F("loadPerformance() - Failed to read file:"));
        Serial.println(filename);
        return;
    }
    // Copy values from the JsonDocument to the PerformanceStruct
    strncpy(currentPerformance.PerformanceName, doc["PerformanceName"], sizeof(currentPerformance.PerformanceName));
    currentPerformance.UID = doc["UID"];
    currentPerformance.mode = doc["Mode"];
    // TODO
    file.close();
}

FLASHMEM void savePerformance()
{
    StaticJsonDocument<1024> doc;
    // Need to generate a new UID as the performance settings may have changed if overwriting an existing performance
    String output;
    serializeJson(doc, output);      // Generate JSON without UID
    uint32_t iUID = getHash(output); // Generate UID
    currentPerformance.UID = iUID;   // Give current patch a UID
    doc["UID"] = iUID;               // Insert UID
    serializeJson(doc, output);      // Generate complete JSON to save

    char result[30];
    concatPerformanceFolderAndUID(iUID, result);
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