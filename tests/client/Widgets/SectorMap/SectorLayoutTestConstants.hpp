#pragma once

#include <SFML/System/Vector2.hpp>
#include <array>
#include <cstdint>
#include "MapView.hpp"
#include "MockupPlacement.hpp"
#include "StarBearing.hpp"

namespace rtype::client::testing {

/**
 * @brief The view of the map zoomed out, and the pixels the kit of the mockup
 * gives four servers in it: node ran place7() of the kit's bundle.js on
 * view7(1, 24, 136, {R: 60, spread: true, kind: 'axis'}).
 */
constexpr MapView MOCKUP_ZOOMED_OUT{
    .zoom = 1.0, .centerX = 24.0, .centerY = 136.0};
constexpr std::array<MockupPlacement, 4> MOCKUP_PLACEMENTS{
    MockupPlacement{.bearing = {.pingMilliseconds = 24, .angleDegrees = -15},
                    .pixel = {173, 120}},
    MockupPlacement{.bearing = {.pingMilliseconds = 88, .angleDegrees = -25},
                    .pixel = {283, 110}},
    MockupPlacement{.bearing = {.pingMilliseconds = 225, .angleDegrees = -60},
                    .pixel = {414, 73}},
    MockupPlacement{.bearing = {.pingMilliseconds = 16, .angleDegrees = 75},
                    .pixel = {161, 216}}};

/** @brief The reach of the planet and its ring to the right of it, from the
 * reach() of the kit, and how close the test reads it. */
constexpr double MOCKUP_REACH_TO_THE_RIGHT = 109.583410;
constexpr double REACH_TOLERANCE = 1e-6;

/** @brief The chosen server of the zoom of the mockup, the centre of the planet
 * its viewOn7() gives at zoom 2, and where another server lands in that view.
 */
constexpr StarBearing ORBITAL_BEARING{.pingMilliseconds = 24,
                                      .angleDegrees = -15};
constexpr sf::Vector2i CHOSEN_STAR_PIXEL{300, 126};
constexpr double MOCKUP_ZOOMED_IN_CENTER_X = 2.0;
constexpr double MOCKUP_ZOOMED_IN_CENTER_Y = 159.0;
constexpr StarBearing NEBULA_BEARING{.pingMilliseconds = 41,
                                     .angleDegrees = 40};
constexpr sf::Vector2i NEBULA_PIXEL_ZOOMED_IN{405, 244};

/** @brief A ping at the floor of the spread, which the kit places just past
 * the planet, and one below it. */
constexpr StarBearing FLOOR_BEARING{.pingMilliseconds = 9, .angleDegrees = 0};
constexpr sf::Vector2i FLOOR_PIXEL{134, 136};
constexpr StarBearing BELOW_FLOOR_BEARING{.pingMilliseconds = 4,
                                          .angleDegrees = 0};

/** @brief Servers whose directions are compared. */
constexpr std::int64_t FIRST_SERVER = 7;
constexpr std::int64_t SECOND_SERVER = 8;
constexpr double LOW_PING_MILLISECONDS = 24.0;
constexpr int SERVERS_TO_SPREAD = 50;

/** @brief A far ping, and the widest direction the map shows at it:
 * asin(78 / (225 * 1.75)) is 11.43 degrees. */
constexpr double FAR_PING_MILLISECONDS = 225.0;
constexpr int WIDEST_ANGLE_AT_THE_FAR_PING = 11;
constexpr int WIDEST_ANGLE = 75;

}  // namespace rtype::client::testing
