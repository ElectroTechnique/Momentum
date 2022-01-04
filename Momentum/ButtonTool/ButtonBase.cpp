#include "ButtonBase.h"

namespace ButtonTool
{

    ButtonBase &ButtonBase::attachButtonCallback(btnCallback_t cb)
    {
        btnCallback = cb;
        return *this;
    }

    void ButtonBase::update(uint32_t btn)
    {
        if (button.update(btn))
        {
            btnChanged = true;
            if (btnCallback != nullptr)
            {
                btnCallback(button.read());
            }
        }
    }
}
