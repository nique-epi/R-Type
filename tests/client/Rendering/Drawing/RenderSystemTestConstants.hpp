#pragma once

#include <SFML/System/Vector2.hpp>
#include <chrono>
#include "Layer.hpp"
#include "RenderingConstants.hpp"
#include "SpriteFrame.hpp"
#include "TimeConstants.hpp"

/** @brief Where a scene spawns its entities, and where a test moves one. */
constexpr float SPAWN_X = 1.0F;
constexpr float SPAWN_Y = 2.0F;
constexpr float SHIP_X = 40.0F;
constexpr float SHIP_Y = 25.0F;

/** @brief Gameplay values an entity may carry, which drawing ignores. */
constexpr float SPEED = 5.0F;
constexpr float BOX_SIZE = 9.0F;

/** @brief The frame of a whole 16 × 16 image, and a cell of a sheet. */
constexpr rtype::game::SpriteFrame WHOLE_IMAGE{
    .left = 0, .top = 0, .width = 16, .height = 16};
constexpr rtype::game::SpriteFrame SHEET_CELL{
    .left = 40, .top = 72, .width = 24, .height = 8};

/** @brief The first value past the last layer, as a corrupted layer would
 * carry. */
constexpr auto FIRST_VALUE_AFTER_THE_LAST_LAYER =
    static_cast<rtype::game::Layer>(rtype::client::LAYER_COUNT);

/** @brief How long a nudge lasts, and how long a vibrating entity stays on
 * each side. */
constexpr rtype::engine::Duration NUDGE_TIME = std::chrono::milliseconds(50);
constexpr rtype::engine::Duration VIBRATION_STEP_TIME =
    std::chrono::milliseconds(16);

/** @brief How long an invulnerable ship stays on each side of its blink. */
constexpr rtype::engine::Duration BLINK_STEP_TIME =
    std::chrono::milliseconds(67);

/** @brief The length of a reveal, and two ages of it: in its first half, and
 * at three quarters. */
constexpr rtype::engine::Duration A_REVEAL = std::chrono::milliseconds(200);
constexpr rtype::engine::Duration A_QUARTER_OF_A_REVEAL =
    std::chrono::milliseconds(50);
constexpr rtype::engine::Duration THREE_QUARTERS_OF_A_REVEAL =
    std::chrono::milliseconds(150);

/** @brief Half of the 16 rows of WHOLE_IMAGE. */
constexpr int HALF_OF_THE_ROWS = 8;

/** @brief A nudge of one pixel to the right. */
constexpr sf::Vector2i ONE_PIXEL_RIGHT(1, 0);
