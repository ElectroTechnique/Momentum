#pragma once

#include "Bounce2.h"

namespace EncoderTool
{
    class EncoderButton : public Bounce
    {
    public:
        EncoderButton() : Bounce() {}

        bool update(bool state)
        {
            curState = state;
            return Bounce::update();
        }

        // bool longPress()
        // {
        //     return durationOfPreviousState() > 100;
        // }

    protected:
        bool readCurrentState() override { return curState; }
        bool curState;
    };
}