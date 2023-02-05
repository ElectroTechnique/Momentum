#pragma once

#include "ButtonPlexBase.h"

/*
Modified from https://github.com/luni64/EncoderTool to support buttons on 74HC595 parallel shift registers
The tick() code has been written specifically for Momentum due to the three parallel shift registers
*/

namespace ButtonTool
{
    class ButtonPlex74165 : public ButtonPlexBase
    {
    public:
        inline ButtonPlex74165(unsigned nrOfButtons, unsigned pinLD, unsigned pinCLK, unsigned pinA = UINT32_MAX, unsigned pinB = UINT32_MAX, unsigned pinC = UINT32_MAX);
        inline ~ButtonPlex74165();

        inline void begin();
        inline void begin(allBtnCallback_t callback);
        inline void read(); // call as often as possible

    protected:
        const unsigned BtnA,
            BtnB,
            BtnC,
            LD,
            CLK;
        int32_t previousState[8] = {LOW};
        uint32_t WINDOWTIME[7] = {500, 390, 330, 220, 140, 80, 20};
        uint32_t starttime[24] = {0};
        boolean buttonHeld[24] = {false};
        inline void readButton(unsigned i);
        uint8_t w = 0;
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

    void ButtonPlex74165::read()
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

        // Read three buttons in parallel in each shift
        unsigned i = 0;
        while (i < buttonCount)
        {
            if (i > 0)
            {
                digitalWriteFast(CLK, HIGH);
                delayNanoseconds(50);
            }
            buttons[i].update(digitalReadFast(BtnA));
            readButton(i);
            i++;
            buttons[i].update(digitalReadFast(BtnB));
            readButton(i);
            i++;
            buttons[i].update(digitalReadFast(BtnC));
            readButton(i);
            i++;
            if (i > 0)
            {
                digitalWriteFast(CLK, LOW);
                delayNanoseconds(50);
            }
        }
    }

    void ButtonPlex74165::readButton(unsigned i)
    {
        if (i == 2 || i == 5 || i > 11)
            return; // Ignore unconnected shift register inputs ***** This is Momentium specific *****

        if (callback != nullptr && buttons[i].buttonChanged())
        {
            if (buttons[i].getButton() == LOW)
            {
                starttime[i] = millis();
                w = 0;
            }
            else
            {
                if (!buttonHeld[i])
                    callback(i, buttons[i].getButton());
                buttonHeld[i] = false;
                w = 0;
            }
        }

        if (callback != nullptr && buttons[i].getButton() == LOW && (millis() - starttime[i]) > WINDOWTIME[w])
        {
            buttonHeld[i] = true;
            starttime[i] = millis();
            // Acceleration of callbacks when button is held down
            if (w < 6)
                w++;
            if (w > 1)
            {
                callback(i, HELD_REPEAT); // HELD REPEAT
            }
            else
            {
                callback(i, HELD); // HELD ONCE
            }
        }
    }
}