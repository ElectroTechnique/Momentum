#pragma once

// un-comment the following line if you prefer plain function pointers for callbacks
//#define PLAIN_BTN_CALLBACK

//================================================================================================================

#include <cstdint>

#if not defined(PLAIN_BTN_CALLBACK)
#include <functional>
#endif

namespace ButtonTool
{
#if defined(PLAIN_BTN_CALLBACK)
  using btnCallback_t = void (*)(uint32_t state);
  using allBtnCallback_t = void (*)(uint32_t channel, uint32_t state);
#else
  using btnCallback_t = std::function<void(uint32_t state)>;
  using allBtnCallback_t = std::function<void(uint32_t channel, uint32_t state)>;
#endif
}
