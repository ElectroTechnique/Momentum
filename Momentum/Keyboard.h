#pragma once

#include <stdint.h>

typedef enum MajMin
{
    MAJOR = 0,
    MINOR = 1
} MajMin;

extern const char *SCALE_STR[24];
extern const uint8_t SCALE_STEPS[2][8];

extern const char *KEYBOARD_OCT_STR[7];