#pragma once

#include "CurrentAction.hpp"
#include "HurtBox.hpp"
#include "HurtWindow.hpp"

namespace rtype::game {

/** @returns Whether @p box hurts now, its enemy being in @p current: always
 * for a box of no action, otherwise in the action it names, inside its
 * window. */
constexpr bool hurtsNow(const HurtBox& box, const CurrentAction* current) {
  if (!box.activeIn.has_value()) {
    return true;
  }
  if (current == nullptr || current->key != box.activeIn) {
    return false;
  }
  const HurtWindow& window = box.window;
  return current->since >= window.from &&
         (!window.until.has_value() || current->since < *window.until);
}

}  // namespace rtype::game
