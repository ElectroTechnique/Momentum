#pragma once

#include "ButtonConfig.h"
#include "ButtonBase.h"

/*
Modified from https://github.com/luni64/EncoderTool to support buttons on 74HC595 parallel shift registers
This is mostly Momentum specific and includes references to Button numbers
*/
// Button numbers as they are connected to the shift registers
#define VOL_DOWN 0
#define VOL_UP 1
#define BUTTON_8 3
#define BUTTON_7 4
#define BUTTON_5 6
#define BUTTON_4 7
#define BUTTON_6 8
#define BUTTON_2 9
#define BUTTON_1 10
#define BUTTON_3 11

namespace ButtonTool
{
    class ButtonPlexBase
    {
    public:
        void attachCallback(allBtnCallback_t callback);
        ButtonBase &operator[](std::size_t idx);

    protected:
        ButtonPlexBase(unsigned ButtonCount);
        ~ButtonPlexBase();

        void begin();
        void begin(allBtnCallback_t);

        const uint32_t buttonCount;
        ButtonBase *buttons;

        allBtnCallback_t callback = nullptr;
    };
}
