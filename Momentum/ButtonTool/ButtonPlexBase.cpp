#include "ButtonPlexBase.h"

namespace ButtonTool
{
    ButtonPlexBase::ButtonPlexBase(unsigned eCnt) : buttonCount(eCnt)
    {
        buttons = new ButtonBase[eCnt];
    }

     void ButtonPlexBase::begin()
     {
     }

    ButtonPlexBase::~ButtonPlexBase()
    {
        delete[] buttons;
    }

    ButtonBase &ButtonPlexBase::operator[](std::size_t idx)
    {
        return idx < buttonCount ? buttons[idx] : buttons[buttonCount - 1];
    }

    void ButtonPlexBase::attachCallback(allBtnCallback_t _callback)
    {
        callback = _callback;
    }
}
