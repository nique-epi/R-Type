#pragma once

#include "ScreenId.hpp"

namespace rtype::client {

/**
 * @brief What a screen may ask of the screens around it: open one, close the
 * one on top, or replace them all.
 *
 * A request only takes effect once the frame loop applies it, between two
 * events, so a screen may ask to leave while it handles an event. Screens get
 * this interface, never the stack itself: none of them can apply the requests
 * or update, draw or hand events to the other screens.
 */
class IScreenNavigator {
 public:
  virtual ~IScreenNavigator() = default;

  /** @brief Asks for a screen to be put over the others, which stay below
   * it. */
  virtual void push(ScreenId screen) = 0;

  /** @brief Asks for the screen on top to be removed, whichever screen asks;
   * nothing happens if no screen is left by then. */
  virtual void pop() = 0;

  /** @brief Asks for every screen to be destroyed, from the top down, then for
   * the given one to be built as the only one. */
  virtual void replaceAll(ScreenId screen) = 0;
};

}  // namespace rtype::client
