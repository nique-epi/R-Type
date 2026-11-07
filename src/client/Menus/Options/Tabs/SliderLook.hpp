#pragma once

#include "Slider.hpp"

namespace rtype::client {

/** @brief How a slider of the options reads: its range and its steps, and the
 * unit written after its value, the two in four characters at most
 * (WIDEST_SLIDER_VALUE). */
struct SliderLook {
  Slider::Range range;
  const char* unit = "";
};

}  // namespace rtype::client
