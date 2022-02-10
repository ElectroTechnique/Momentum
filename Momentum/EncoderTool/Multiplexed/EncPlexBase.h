#pragma once

#include "../config.h"
#include "../EncoderBase.h"

namespace EncoderTool
{
    class EncPlexBase
    {
    public:
        void attachCallback(allCallback_t callback);
        void attachBtnCallback(allBtnCallback_t btnCallback);
        EncoderBase &operator[](std::size_t idx);

    protected:
        EncPlexBase(unsigned EncoderCount);
        ~EncPlexBase();

        void begin(CountMode mode = CountMode::quarter);
        void begin(allCallback_t, allBtnCallback_t, CountMode mode = CountMode::quarter);

        const uint32_t encoderCount;
        EncoderBase *encoders;

        allCallback_t callback = nullptr;
        allBtnCallback_t btnCallback = nullptr;

        // Encoder acceleration
        long ACC_TIME = 10; // 10ms
        long ACC_TIME2 = 3; // 3ms
        long last[8] = {0};
    };
}
