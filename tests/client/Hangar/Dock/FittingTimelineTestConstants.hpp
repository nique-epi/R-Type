#pragma once

#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <cstdint>
#include "FittingRoute.hpp"
#include "FittingRun.hpp"

namespace rtype::client::testing {

/** @brief A route: the hull at (10, 10), off screen at (500, 10), the mount
 * at (100, 50), approached from 16 pixels above. */
constexpr sf::Vector2i HULL_GRIP{10, 10};
constexpr sf::Vector2i OFF_SCREEN{500, 10};
constexpr sf::Vector2i MOUNT_POINT{100, 50};
constexpr FittingRoute A_ROUTE{.grip = HULL_GRIP,
                               .away = OFF_SCREEN,
                               .place = MOUNT_POINT,
                               .approach = -16};

/** @brief Moments of a fitting, as shares of its length: its start, halfway
 * back from off screen, just before the item touches the ship, when it does
 * (0.72), the old hat gone but the new one not set yet (0.65), its end. */
constexpr float START = 0.0F;
constexpr float HALFWAY_BACK = 0.5F;
constexpr float JUST_BEFORE_PLACING = 0.71F;
constexpr float PLACING = 0.72F;
constexpr float BETWEEN_HATS = 0.65F;
constexpr float END = 1.0F;

/** @brief A ship wearing the party hat that gets the top hat, the first item
 * of the locker; what the head holds for each. */
constexpr std::size_t TOP_HAT = 0;
constexpr std::uint8_t WEARS_PARTY_HAT = 2;
constexpr std::uint8_t WEARS_TOP_HAT = 1;
constexpr std::uint8_t WEARS_NOTHING = 0;
constexpr FittingRun TOP_HAT_OVER_PARTY_HAT{
    .item = TOP_HAT, .started = {}, .before = {.head = WEARS_PARTY_HAT}};

}  // namespace rtype::client::testing
