#include "Keyboard.h"
#include "Arduino.h"

const char *SCALE_STR[24] PROGMEM = {"C maj", "C min", "C# maj", "C# min",
                                     "D maj", "D min", "D# maj", "D# min",
                                     "E maj", "E min",
                                     "F maj", "F min", "F# maj", "F# min",
                                     "G maj", "G min", "G# maj", "G# min",
                                     "A maj", "A min", "A# maj", "A# min",
                                     "B maj", "B min"};

const uint8_t SCALE_STEPS[2][8]  = {{0, 2, 4, 5, 7, 9, 11, 12},  // Maj
                                           {0, 2, 3, 5, 7, 8, 10, 12}}; // Min

const char *KEYBOARD_OCT_STR[7] = {"3 below", "2 below", "1 below", "Middle", "1 above", "2 above", "3 above"};