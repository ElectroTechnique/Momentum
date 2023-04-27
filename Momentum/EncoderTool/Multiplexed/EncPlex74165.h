#pragma once

#include "Bounce2.h"
#include "EncPlexBase.h"

/*
Modified from https://github.com/luni64/EncoderTool
This has encoder acceleration and an all button callback added.
Some parts of this library have been removed.
The tick() code has been rewritten specifically for Momentum
*/

namespace EncoderTool
{
    class EncPlex74165 : public EncPlexBase
    {
    public:
        inline EncPlex74165(unsigned nrOfEncoders, unsigned pinLD, unsigned pinCLK, unsigned pinA, unsigned pinB, unsigned pinBtn = UINT32_MAX);
        inline ~EncPlex74165();

        inline void begin(CountMode mode = CountMode::quarter);
        inline void begin(allCallback_t callback, allBtnCallback_t btnCallback, CountMode m = CountMode::quarter);
        inline void read(); // call as often as possible

    protected:
        const unsigned A, B, Btn, LD, CLK;
        uint32_t now = 0;
        int32_t previousState[8] = {LOW};
        uint32_t WINDOWTIME[7] = {500, 390, 330, 220, 140, 80, 20};
        uint32_t btnStarttime[8] = {0};
        boolean buttonHeld[8] = {false};
        uint8_t w = 0;
        boolean encRotationBlocked = false;
    };

    // IMPLEMENTATION ============================================

    EncPlex74165::EncPlex74165(unsigned nrOfEncoders, unsigned pinLD, unsigned pinCLK, unsigned pinA, unsigned pinB, unsigned pinBtn)
        : EncPlexBase(nrOfEncoders), A(pinA), B(pinB), Btn(pinBtn), LD(pinLD), CLK(pinCLK)
    {
    }

    EncPlex74165::~EncPlex74165()
    {
        for (unsigned pin : {A, B, Btn, LD, CLK})
        {
            pinMode(pin, INPUT_DISABLE);
        };
    }

    void EncPlex74165::begin(allCallback_t cb, allBtnCallback_t bcb, CountMode mode)
    {
        begin(mode);
        attachCallback(cb);
        attachBtnCallback(bcb);
    }

    void EncPlex74165::begin(CountMode mode)
    {
        EncPlexBase::begin(mode);

        for (uint8_t pin : {A, B, Btn})
        {
            pinMode(pin, INPUT);
        }
        for (uint8_t pin : {LD, CLK})
        {
            pinMode(pin, OUTPUT);
        }

        digitalWriteFast(LD, HIGH); // active low
        delayMicroseconds(1);
    }

    void EncPlex74165::read()
    {
        // load current values to shift register
        digitalWriteFast(LD, LOW);
        delayNanoseconds(50);
        delayNanoseconds(50);
        delayNanoseconds(50);
        digitalWriteFast(LD, HIGH);

        now = millis();

        for (unsigned i = 0; i < encoderCount; i++)
        {
            if (i > 0)
            {
                digitalWriteFast(CLK, HIGH);
                delayNanoseconds(50);
            }
            // Don't read first four because encoders are on last four positions.
            if (i > 3)
            {
                int delta = encoders[i].update(digitalReadFast(A), digitalReadFast(B), digitalReadFast(Btn));
                if (callback != nullptr && encoders[i].buttonChanged())
                {
                    if (encoders[i].getButton() == LOW)
                    {
                        btnStarttime[i] = millis();
                        w = 0;
                        encRotationBlocked = true;//Prevent rotation of encoder when pressing button
                    }
                    else
                    {
                        if (!buttonHeld[i])
                            btnCallback(i, encoders[i].getButton());
                        buttonHeld[i] = false;
                        w = 0;
                        encRotationBlocked = false;
                    }
                }

                else if (delta != 0 && callback != nullptr && !encRotationBlocked)
                {
                    callback(i, encoders[i].getValue(), delta);
                    if ((now - last[i]) < ACC_TIME) // Accelerate 1
                    {
                        callback(i, encoders[i].getValue(), delta);
                        if ((now - last[i]) < ACC_TIME2) // Accelerate 2
                        {
                            callback(i, encoders[i].getValue(), delta);
                            callback(i, encoders[i].getValue(), delta);
                            callback(i, encoders[i].getValue(), delta);
                        }
                    }
                    last[i] = now;
                }

                if (btnCallback != nullptr && encoders[i].getButton() == LOW && (millis() - btnStarttime[i]) > WINDOWTIME[w])
                {
                    buttonHeld[i] = true;
                    btnStarttime[i] = millis();
                    // Acceleration of callbacks when button is held down
                    if (w < 6)
                        w++;
                    if (w > 1)
                    {
                        btnCallback(i, HELD_REPEAT); // HELD REPEAT
                    }
                    else
                    {
                        btnCallback(i, HELD); // HELD ONCE
                    }
                }
            }
            // Clock after first is read
            if (i > 0)
            {
                digitalWriteFast(CLK, LOW);
                delayNanoseconds(50);
            }
        }
    }

} // namespace EncoderTool