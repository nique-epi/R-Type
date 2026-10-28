#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <chrono>
#include "BackgroundColorConstants.hpp"
#include "InterfaceColorConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief A bay of the waiting room, and the space between two of them, in
 * pixels: four bays side by side fill the width of the safe margin. */
constexpr sf::Vector2i HANGAR_BAY_SIZE(102, 112);
constexpr int HANGAR_BAY_GAP = 8;

/**
 * @brief Inside a bay, from its top-left corner: the player number, the tag
 * of the local player as far from the right side, the band of the ship color,
 * the top of the ship enlarged BAY_SHIP_SCALE times, the row of the nickname
 * and the cell that says whether the pilot is ready. As on the picture of the
 * waiting room the team chose.
 */
constexpr sf::Vector2i BAY_NUMBER_OFFSET(8, 8);
constexpr sf::IntRect BAY_COLOR_BAND({8, 20}, {86, 2});
constexpr int BAY_SHIP_SCALE = 2;
constexpr int BAY_SHIP_TOP = 30;
constexpr int BAY_NICKNAME_TOP = 70;
constexpr sf::IntRect BAY_STATE_CELL({8, 86}, {86, 16});

/**
 * @brief Beacons of an open bay: BEACON_COUNT squares in a row across the
 * middle of the place of the ship, BEACON_SPACING apart, one lit at a time
 * for BEACON_STEP, from left to right, the way a ship comes in.
 */
constexpr int BEACON_COUNT = 8;
constexpr int BEACON_SIZE = 2;
constexpr sf::Vector2i FIRST_BEACON(19, 47);
constexpr int BEACON_SPACING = 9;
constexpr engine::Duration BEACON_STEP = std::chrono::milliseconds(200);
constexpr sf::Color BEACON_OFF_COLOR = DEEP_BLUE;
constexpr sf::Color BEACON_LIT_COLOR = HUD_CYAN;

/** @brief Docking clamps of an open bay: a stroke down each side of the
 * beacons, an arm CLAMP_ARM_LENGTH long at each end, pointing inwards. */
constexpr int LEFT_CLAMP_COLUMN = 14;
constexpr int RIGHT_CLAMP_COLUMN = 87;
constexpr int CLAMP_TOP = 34;
constexpr int CLAMP_HEIGHT = 28;
constexpr int CLAMP_ARM_LENGTH = 4;
constexpr sf::Color CLAMP_COLOR = BLUE;

/** @brief Texts of a bay: an open bay, and what the cell of a pilot says.
 * The player number and the tag of the local player are written as
 * PLAYER_NUMBER_PREFIX and LOCAL_PLAYER_TAG say. */
constexpr const char* OPEN_BAY_TEXT = "OPEN BAY";
constexpr const char* READY_TEXT = "READY";
constexpr const char* READY_ASKED_TEXT = "READY...";
constexpr const char* NOT_READY_TEXT = "NOT READY";
constexpr const char* NOT_READY_ASKED_TEXT = "CANCEL...";

}  // namespace rtype::client
