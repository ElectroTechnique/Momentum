#pragma once

#include "ButtonConfig.h"
#include "ButtonBase.h"

namespace ButtonTool
{
    class ButtonPlexBase
    {
     public:
        void attachCallback(allBtnCallback_t callback);
        ButtonBase& operator[](std::size_t idx);

     protected:
        ButtonPlexBase(unsigned ButtonCount);
        ~ButtonPlexBase();

        void begin();
        void begin(allBtnCallback_t);

        const uint32_t buttonCount;
        ButtonBase* buttons;

        allBtnCallback_t callback = nullptr;
    };
}
