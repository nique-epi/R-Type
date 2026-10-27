#pragma once

#include <SFML/System/Vector2.hpp>
#include <chrono>
#include <cstdint>
#include <string_view>
#include "TimeConstants.hpp"

namespace rtype::client::testing {

/** @brief The game of the design system: ACE (1, this client, 12450),
 * CAPTAIN_STARDUST (2, 31800), NOVA (3, 8200). */
constexpr std::uint8_t ACE_NUMBER = 1;
constexpr std::uint8_t STARDUST_NUMBER = 2;
constexpr std::uint8_t NOVA_NUMBER = 3;
constexpr std::uint32_t ACE_SCORE = 12450;
constexpr std::uint32_t STARDUST_SCORE = 31800;
constexpr std::uint32_t NOVA_SCORE = 8200;
constexpr std::string_view ACE_NICKNAME = "ACE";
constexpr std::string_view STARDUST_NICKNAME = "CAPTAIN_STARDUST";
constexpr std::string_view NOVA_NICKNAME = "NOVA";
/** @brief Their ranks: CAPTAIN_STARDUST first, ACE second, NOVA third. */
constexpr int STARDUST_RANK = 1;
constexpr int ACE_RANK = 2;
constexpr int NOVA_RANK = 3;
constexpr std::uint32_t END_SEED = 7;

/** @brief Middles of the choices, in pixels of the window as the fixture
 * maps them (one pixel of the picture is INTERFACE_PIXEL of the window): the
 * call under the book, QUIT under its right corner, and a point on neither. */
constexpr sf::Vector2i PROMPT_MIDDLE_PIXEL(240, 248);
constexpr sf::Vector2i QUIT_MIDDLE_PIXEL(421, 231);
constexpr sf::Vector2i NOWHERE_PIXEL(10, 10);

/** @brief The first sound of the book: its landing, 0.4 s after the screen
 * opened (card "A v2"). */
constexpr rtype::engine::Duration DESIGN_LANDING =
    std::chrono::milliseconds(400);

}  // namespace rtype::client::testing
