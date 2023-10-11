
#include "FlasherX/FlasherX.h"

Stream *serial = &Serial;

void firmwareUpdate()
{

    SD.setMediaDetectPin(pinCD);
    cardStatus = SD.begin(BUILTIN_SDCARD);
    if (SD.mediaPresent()) // Card is detected by CD pin
    {
        // ++++ For older sd cards - keep trying
        if (!cardStatus)
            cardStatus = SD.begin(BUILTIN_SDCARD);
        if (!cardStatus)
            cardStatus = SD.begin(BUILTIN_SDCARD);
        if (!cardStatus)
            cardStatus = SD.begin(BUILTIN_SDCARD);
        //++++
    }
    if (cardStatus)
    {

        Serial.println(F("SD card is connected"));

        FsFile file;
        file = SD.sdfs.open(HEXFILENAME, O_READ);
        if (!file)
        {
            Serial.println(F("No firmware on SD card"));
        }
        else
        {
            Serial.println(F("++++  Firmware update   ++++"));
            state = State::FIRMWAREUPDATE;
            setup_flasherx();
            start_upgrade(&file);
            file.close();
        }
    }
    else
    {
        Serial.println(F("SD card is not connected or unusable"));
    }
}