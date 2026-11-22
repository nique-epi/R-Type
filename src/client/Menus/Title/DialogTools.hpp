#pragma once

#include <functional>
#include "IScreenNavigator.hpp"
#include "ISoundPlayer.hpp"
#include "Language.hpp"

namespace rtype::client {

/** @brief What a dialog over the title is built with: the stack it answers
 * on, the sounds of its buttons, and the language of its words. */
struct DialogTools {
  std::reference_wrapper<IScreenNavigator> navigator;
  std::reference_wrapper<ISoundPlayer> sounds;
  Language language = Language::English;
};

}  // namespace rtype::client
