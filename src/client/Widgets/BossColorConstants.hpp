#pragma once

#include <SFML/Graphics/Color.hpp>
#include <cstdint>
#include "EffectColorConstants.hpp"

namespace rtype::client {

/** @brief The colors of the boss: the pink of its eye, which fills its bar;
 * the lag of the points just lost, BYDO_PALE; an empty gauge, SMOKE; the red
 * of its rage and the hot pixels flickering in it; the marks (name, labels,
 * waiting border), and the same marks dimmed on a spent gauge. */
constexpr sf::Color BYDO_EYE_PINK(255, 90, 122);
constexpr sf::Color BOSS_LAG_COLOR = BYDO_PALE;
constexpr sf::Color BOSS_EMPTY_COLOR = SMOKE;
constexpr sf::Color RAGE_RED(255, 61, 46);
constexpr sf::Color RAGE_HOT(255, 214, 160);
constexpr sf::Color BOSS_MARK_COLOR(227, 211, 166);
constexpr std::uint8_t BOSS_SPENT_ALPHA = 96;
constexpr sf::Color BOSS_SPENT_MARK_COLOR(BOSS_MARK_COLOR.r, BOSS_MARK_COLOR.g,
                                          BOSS_MARK_COLOR.b, BOSS_SPENT_ALPHA);

}  // namespace rtype::client
