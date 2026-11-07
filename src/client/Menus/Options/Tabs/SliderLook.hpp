#pragma once

#include "SettingsConstants.hpp"

namespace rtype::client {

/** @brief How a slider of the chat reads beside the range of its setting: the
 * step one press moves it by, and the unit written after its value, the two in
 * four characters at most (WIDEST_SLIDER_VALUE). */
struct SliderLook {
  int step = CHAT_SETTING_STEP;
  const char* unit = "";
};

}  // namespace rtype::client
