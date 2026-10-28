#pragma once

#include <SFML/Window/Keyboard.hpp>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include "PlayerLeft.hpp"
#include "TimeConstants.hpp"

/** @brief A key that is not the options key. */
constexpr sf::Keyboard::Key OTHER_KEY = sf::Keyboard::Key::Space;

/** @brief How many presses the system repeats while a key is held down. */
constexpr int KEY_REPEATS = 3;

/** @brief How many times the entity layer is painted: when the game screen is
 * built, then after its first update. */
constexpr int PAINTS_AT_CONSTRUCTION = 1;
constexpr int PAINTS_AFTER_ONE_UPDATE = 2;

/** @brief How many times a picture is laid on the target after one draw. */
constexpr int SHOWS_AFTER_ONE_DRAW = 1;

/** @brief How many entities a world state of the tests lists, a ship and a
 * Bydo, and how many are left once one is removed. */
constexpr std::size_t ENTITIES_LISTED = 2;
constexpr std::size_t ENTITIES_LEFT_AFTER_ONE_REMOVAL = 1;

/** @brief The actions held, as the bits of an input: none, up, shoot. */
constexpr std::uint8_t NOTHING_HELD = 0;
constexpr std::uint8_t UP_BIT = 0b00001;
constexpr std::uint8_t SHOOT_BIT = 0b10000;

/** @brief One long frame, as after the window was dragged. */
constexpr rtype::engine::Duration LONG_FRAME = std::chrono::milliseconds(500);

/** @brief The numbers of the pilots of the waiting room, ACE, NOVA and KAI,
 * and the number of this client: NOVA. */
constexpr std::uint8_t FIRST_PILOT = 1;
constexpr std::uint8_t SECOND_PILOT = 2;
constexpr std::uint8_t THIRD_PILOT = 3;
constexpr std::uint8_t LOCAL_NUMBER = SECOND_PILOT;
constexpr std::string_view FIRST_PILOT_NICKNAME = "ACE";
constexpr std::string_view SECOND_PILOT_NICKNAME = "NOVA";
constexpr std::string_view THIRD_PILOT_NICKNAME = "KAI";

/** @brief The lives and the score a world state gives the first pilot. */
constexpr std::uint8_t PILOT_LIVES = 3;
constexpr std::uint32_t PILOT_SCORE = 12400;

/** @brief Scores of the end of a game: ACE 100, NOVA (this client) 300, KAI
 * 200; how many pilots it ranks; and the second the game goes on before the
 * end screen (the design: the end screen starts within a second). */
constexpr std::uint32_t FIRST_PILOT_FINAL = 100;
constexpr std::uint32_t SECOND_PILOT_FINAL = 300;
constexpr std::uint32_t THIRD_PILOT_FINAL = 200;
constexpr std::size_t PILOTS_AT_THE_END = 3;
constexpr rtype::engine::Duration END_WAIT = std::chrono::milliseconds(1000);

/** @brief How many times the status canvas is displayed when the game screen
 * is built. */
constexpr int DISPLAYS_AT_CONSTRUCTION = 1;

/** @brief The server says the third pilot left, three times, as it repeats
 * a PlayerLeft. */
constexpr rtype::network::PlayerLeft THIRD_PILOT_LEFT{
    .playerNumber = THIRD_PILOT,
    .reason = rtype::network::DepartureReason::Left};
constexpr int PLAYER_LEFT_COPIES = 3;
