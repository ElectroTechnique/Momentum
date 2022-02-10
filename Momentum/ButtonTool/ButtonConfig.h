#pragma once

#include <cstdint>
#include <functional>

namespace ButtonTool
{
  using btnCallback_t = std::function<void(uint32_t state)>;
  using allBtnCallback_t = std::function<void(uint32_t channel, uint32_t state)>;
}
