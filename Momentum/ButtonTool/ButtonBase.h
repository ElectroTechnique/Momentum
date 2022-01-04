#pragma once
#include "Button.h"
#include "ButtonConfig.h"

namespace ButtonTool
{
  class ButtonBase
  {
    public:
      void begin();
  
      ButtonBase &attachButtonCallback(btnCallback_t);
  
      int32_t getButton() {
        return button.read();
      }
      bool buttonChanged()
      {
        bool ret = btnChanged;
        btnChanged = false;
        return ret;
      }
  
      void update(uint32_t btn = 0);
  
    protected:
      ButtonBase() = default;
  
      Button button;
      bool btnChanged = false;
      btnCallback_t btnCallback = nullptr;
  
      ButtonBase &operator=(ButtonBase const &) = delete;
      ButtonBase(ButtonBase const &) = delete;
  
      friend class ButtonPlexBase;
  };
}
