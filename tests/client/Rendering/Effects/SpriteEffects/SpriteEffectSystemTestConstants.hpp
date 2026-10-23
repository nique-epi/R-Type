#pragma once

#include <SFML/System/Vector2.hpp>
#include <chrono>
#include <string_view>
#include "Position.hpp"
#include "TimeConstants.hpp"

/** @brief Where the entities of the tests stand, and where a ship moves to. */
constexpr rtype::game::Position START{.x = 800.0F, .y = 400.0F};
constexpr rtype::game::Position MOVED{.x = 900.0F, .y = 300.0F};

/** @brief How long the nudges and lifetimes of the tests last. */
constexpr rtype::engine::Duration EFFECT_TIME = std::chrono::milliseconds(100);

/** @brief Where a muzzle flash stands from the ship it is anchored to. */
constexpr sf::Vector2f NOSE_OFFSET(40.0F, 4.0F);

/** @brief The text of the figure of a gain the tests show. */
constexpr std::string_view GAIN_FIGURE_TEXT = "+100";
