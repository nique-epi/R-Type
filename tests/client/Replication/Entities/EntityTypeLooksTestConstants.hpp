#pragma once

#include <cstdint>
#include <string_view>
#include "ShipGun.hpp"

namespace rtype::client::testing {

/** @brief The type the looks of the tests draw, a type they do not, and the
 * numbers the table of the tests gives them. */
constexpr std::string_view LOOKED_UP_TYPE_NAME = "drifter";
constexpr std::string_view TYPE_WITHOUT_APPEARANCE_NAME = "voltjelly";
constexpr std::uint8_t LOOKED_UP_TYPE_NUMBER = 1;
constexpr std::uint8_t TYPE_WITHOUT_APPEARANCE_NUMBER = 2;
constexpr std::uint8_t NUMBER_PAST_THE_TABLE = 3;

/** @brief The box the table of the tests gives LOOKED_UP_TYPE_NAME, and the
 * gun of its ships. */
constexpr std::uint16_t LOOKED_UP_BOX_WIDTH = 96;
constexpr std::uint16_t LOOKED_UP_BOX_HEIGHT = 48;
constexpr network::ShipGun LOOKED_UP_SHIP_GUN{.shipType = 0,
                                              .projectileType = 0,
                                              .intervalTicks = 20,
                                              .firstShotTicks = 0,
                                              .velocityX = 0.0F,
                                              .velocityY = 0.0F};

/** @brief The sheet of LOOKED_UP_TYPE_NAME, as the client reads it. */
constexpr std::string_view LOOKED_UP_SHEET_TEXT = R"(
[gameplay]

[appearance]
sheet = "sprites/entities.png"

[appearance.idle]
left = 0
top = 72
width = 24
height = 24
frames = 2
spacing = 2
frame_ms = 250
)";

}  // namespace rtype::client::testing
