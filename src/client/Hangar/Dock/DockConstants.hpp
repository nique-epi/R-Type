#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <chrono>

namespace rtype::client {

/** @brief The scale a hull is drawn at on the reference card: pixel for
 * pixel, 32 x 16, where an accessory is drawn larger. */
constexpr int BLUEPRINT_HULL_SCALE = 1;

/** @brief The bay: its area, the ship DOCK_SHIP_SCALE times its size with its
 * top on DOCK_SHIP_TOP, a pixel higher every other DOCK_BOB_STEP. */
constexpr sf::IntRect DOCK_AREA{{176, 34}, {280, 184}};
constexpr int DOCK_SHIP_SCALE = 4;
constexpr int DOCK_SHIP_TOP = 136;
constexpr std::chrono::milliseconds DOCK_BOB_STEP{700};
constexpr int DOCK_BOB_STEPS = 2;

/** @brief The wall of the bay: panels DOCK_PANEL_WIDTH wide, the light from
 * its ceiling, DOCK_LIGHT_WIDTH wide, down DOCK_LIGHT_SHARE of its height, and
 * the row of its beacons. */
constexpr int DOCK_PANEL_WIDTH = 24;
constexpr int DOCK_PANEL_SEAM = 1;
constexpr float DOCK_LIGHT_SHARE = 0.55F;
constexpr int DOCK_LIGHT_WIDTH = 92;
constexpr int DOCK_BEACONS_TOP = 208;

/** @brief The reference card of the chosen item: its area, the pitch of its
 * grid, where its drawing starts, how large, the gap of its size rule, its
 * label. */
constexpr sf::IntRect BLUEPRINT_AREA{{284, 40}, {64, 58}};
constexpr int BLUEPRINT_GRID_PITCH = 4;
/** @brief A row of the grid of the card lights one pixel in this many. */
constexpr int BLUEPRINT_GRID_DOT_STEP = 2;
constexpr int BLUEPRINT_DRAWING_TOP = 13;
constexpr int BLUEPRINT_DRAWING_SCALE = 2;
constexpr int BLUEPRINT_RULE_GAP = 3;
constexpr sf::Vector2i BLUEPRINT_LABEL_OFFSET{4, 4};
constexpr const char* BLUEPRINT_LABEL = "REF";
constexpr const char* BLUEPRINT_SIZE_JOIN = "X";

/** @brief The sparks of an item placed or taken off: SPARK_COUNT squares on
 * a ring growing from SPARK_FIRST_RADIUS by SPARK_SPREAD, flattened. */
constexpr int SPARK_COUNT = 12;
constexpr int SPARK_SIZE = 2;
constexpr float SPARK_FIRST_RADIUS = 4.0F;
constexpr float SPARK_SPREAD = 18.0F;
constexpr float SPARK_FLATTENING = 0.7F;

}  // namespace rtype::client
