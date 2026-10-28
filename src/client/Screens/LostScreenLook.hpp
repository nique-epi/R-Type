#pragma once

#include <cstddef>
#include "BarState.hpp"
#include "ScreenOffLook.hpp"

namespace rtype::client {

/** @brief What the picture of the NO SIGNAL screen shows at one moment: the
 * screen turning off, or the card with its call in one state and as many
 * letters shown. Two equal looks paint the same picture, so the screen paints
 * only when its look changes. */
struct LostScreenLook {
  /** @brief Whether the card shows; before it, the screen turns off. */
  bool card = false;
  ScreenOffLook screenOff;
  BarState call = BarState::Idle;
  /** @brief Letters of the call shown, 0 before it is typed and while it
   * blinks out (see lettersShownOf()). */
  std::size_t callLetters{};

  bool operator==(const LostScreenLook&) const = default;
};

}  // namespace rtype::client
