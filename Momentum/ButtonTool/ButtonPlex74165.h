#pragma once

#include "Bounce2.h"
#include "ButtonPlexBase.h"

namespace ButtonTool
{
    class ButtonPlex74165 : public ButtonPlexBase
    {
    public:
        inline ButtonPlex74165(unsigned nrOfButtons, unsigned pinLD, unsigned pinCLK, unsigned pinA = UINT32_MAX, unsigned pinB = UINT32_MAX, unsigned pinC = UINT32_MAX);
        inline ~ButtonPlex74165();

        inline void begin();
        inline void begin(allBtnCallback_t callback);
        inline void tick(); // call as often as possible

    protected:
        const unsigned BtnA, BtnB, BtnC, LD, CLK;
    };

    // IMPLEMENTATION ============================================

    ButtonPlex74165::ButtonPlex74165(unsigned nrOfButtons, unsigned pinLD, unsigned pinCLK, unsigned pinA, unsigned pinB, unsigned pinC)
        : ButtonPlexBase(nrOfButtons), BtnA(pinA), BtnB(pinB), BtnC(pinC), LD(pinLD), CLK(pinCLK)
    {
    }

    ButtonPlex74165::~ButtonPlex74165()
    {
        for (unsigned pin : {BtnA, BtnB, BtnC, LD, CLK})
        {
            pinMode(pin, INPUT_DISABLE);
        };
    }

    void ButtonPlex74165::begin(allBtnCallback_t cb)
    {
        begin();
        attachCallback(cb);
    }

    void ButtonPlex74165::begin()
    {
        ButtonPlexBase::begin();

        for (uint8_t pin : {BtnA, BtnB, BtnC})
        {
            pinMode(pin, INPUT);
        }
        for (uint8_t pin : {LD, CLK})
        {
            pinMode(pin, OUTPUT);
        }
    }

    void ButtonPlex74165::tick()
    /*
    Button1 SR1 D4
    Button2 SR2 D4
    Button3 SR3 D4
    Button4 SR1 D5
    Button5 SR2 D5
    Button6 SR3 D5
    Button7 SR1 D6
    Button8 SR2 D6
    ButtonUp SR1 D7
    ButtonDown SR2 D7
    */
    {
        // load current values to shift register
        digitalWriteFast(LD, LOW);
        delayNanoseconds(50);
        delayNanoseconds(50);
        delayNanoseconds(50);
        digitalWriteFast(LD, HIGH);

        unsigned i = 0;
        while (i < buttonCount)
        {
            if (i > 0)
            {
                digitalWriteFast(CLK, HIGH);
                delayNanoseconds(50);
            }

            buttons[i].update(digitalReadFast(BtnA));
            if (callback != nullptr && buttons[i].buttonChanged() && buttons[i].getButton() == HIGH)
            {
                callback(i, buttons[i].getButton());
            }
            i++;
            buttons[i].update(digitalReadFast(BtnB));
            if (callback != nullptr && buttons[i].buttonChanged() && buttons[i].getButton() == HIGH)
            {
                callback(i, buttons[i].getButton());
            }
            i++;
            buttons[i].update(digitalReadFast(BtnC));
            if (callback != nullptr && buttons[i].buttonChanged() && buttons[i].getButton() == HIGH)
            {
                callback(i, buttons[i].getButton());
            }
            i++;

            if (i > 0)
            {
                digitalWriteFast(CLK, LOW);
                delayNanoseconds(50);
            }
        }
    }
}
