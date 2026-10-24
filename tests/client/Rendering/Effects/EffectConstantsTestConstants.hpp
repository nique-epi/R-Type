#pragma once

#include <SFML/Graphics/Color.hpp>
#include <array>
#include "EffectConstants.hpp"

/** @brief Every effect of the effect sheet, each with its two versions. */
constexpr std::array ALL_EFFECTS{
    rtype::client::BYDO_EXPLOSION, rtype::client::SHIP_EXPLOSION,
    rtype::client::IMPACT, rtype::client::PLAYER_MUZZLE_FLASH,
    rtype::client::ENEMY_MUZZLE_FLASH};

/** @brief The white at the heart of an explosion, #fffbe8, which no version
 * without flash may show. */
constexpr sf::Color EXPLOSION_WHITE(255, 251, 232);

/** @brief The areas measured in the sheets as shipped: the most pixels a
 * frame of each effect changes between its row and its row without flash,
 * the frames that change any, and the largest silhouette of a Bydo and of a
 * ship. */
constexpr int MEASURED_BYDO_EXPLOSION_FLASH = 275;
constexpr int MEASURED_BYDO_EXPLOSION_FLASH_FRAMES = 1;
constexpr int MEASURED_SHIP_EXPLOSION_FLASH = 23;
constexpr int MEASURED_SHIP_EXPLOSION_FLASH_FRAMES = 8;
constexpr int MEASURED_IMPACT_FLASH = 6;
constexpr int MEASURED_IMPACT_FLASH_FRAMES = 2;
constexpr int MEASURED_MUZZLE_FLASH = 12;
constexpr int MEASURED_MUZZLE_FLASH_FRAMES = 1;
constexpr int MEASURED_BYDO_SILHOUETTE = 367;
constexpr int MEASURED_SHIP_SILHOUETTE = 254;
