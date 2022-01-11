#pragma once

#include "Bounce2.h"

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