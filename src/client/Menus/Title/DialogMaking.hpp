#pragma once

#include <functional>
#include "IScreenNavigator.hpp"
#include "ISoundPlayer.hpp"
#include "TextMetrics.hpp"

namespace rtype::client {

/** @brief What a dialog of the title is made with: the screens around it and
 * the player of its clicks, which must outlive it, and the text size it is
 * laid out at. */
struct DialogMaking {
  std::reference_wrapper<IScreenNavigator> navigator;
  std::reference_wrapper<ISoundPlayer> sounds;
  TextMetrics metrics{};
};

}  // namespace rtype::client
