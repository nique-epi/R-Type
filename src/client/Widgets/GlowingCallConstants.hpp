#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <chrono>
#include <cstdint>
#include "BackgroundColorConstants.hpp"
#include "FireColorConstants.hpp"
#include "InterfaceColorConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief A call to act, written as the end screen writes PRESS ENTER TO
 * REPLAY: one more letter every CALL_LETTER_TIME; once typed and at rest,
 * shown CALL_BLINK_STEPS - 1 steps of CALL_BLINK_STEP and hidden one, counted
 * from its first letter; each letter over a glow of CALL_GLOW pixels in every
 * direction of the grid. The pointer presses it within CALL_HIT_MARGIN of its
 * letters.
 */
constexpr engine::Duration CALL_LETTER_TIME = std::chrono::milliseconds(20);
constexpr engine::Duration CALL_BLINK_STEP = std::chrono::milliseconds(300);
constexpr std::int64_t CALL_BLINK_STEPS = 3;
constexpr int CALL_GLOW = 2;
constexpr sf::Vector2i CALL_HIT_MARGIN(6, 4);

/** @brief The colors of a call: gold with a dark orange glow; under the
 * pointer, yellow with a gold glow; pressed, star white with a gold glow. */
constexpr sf::Color CALL_COLOR = FIRE_GOLD;
constexpr sf::Color CALL_GLOW_COLOR = PLAYER_DARK_ORANGE;
constexpr sf::Color HOVERED_CALL_COLOR = FIRE_YELLOW;
constexpr sf::Color HOVERED_CALL_GLOW_COLOR = FIRE_GOLD;
constexpr sf::Color PRESSED_CALL_COLOR = STAR_WHITE;

}  // namespace rtype::client
