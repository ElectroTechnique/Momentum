#pragma once

#include "Bounce2.h"

#define HELD 2
#define HELD_REPEAT 3
#define HIGH_AFTER_HELD 4

namespace ButtonTool
{
    class Button : public Bounce
    {
    public:
        Button() : Bounce() {}

        bool update(bool state)
        {
            curState = state;
            return Bounce::update();
        }

    protected:
        bool readCurrentState() override { return curState; }
        bool curState;
    };
}