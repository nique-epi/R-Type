#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include "GlowingCallConstants.hpp"
#include "InterfaceConstants.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

/** @returns Where the pointer presses a call whose first letter starts at
 * @p topLeft and whose line is @p width pixels wide: its line, and
 * CALL_HIT_MARGIN around it. constexpr, so the area of each call is a
 * constant. */
constexpr sf::IntRect callArea(sf::Vector2i topLeft, int width) {
  return {topLeft - CALL_HIT_MARGIN,
          {width + (SIDES_PER_AXIS * CALL_HIT_MARGIN.x),
           TEXT_LINE_HEIGHT + (SIDES_PER_AXIS * CALL_HIT_MARGIN.y)}};
}

}  // namespace rtype::client
