#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include "CasinoSettings.hpp"
#include "MillisecondDate.hpp"

/** @brief The casino of the tests: two rooms of eight seats, rounds of 20 s
 * of bets, 6 s of spin and 5 s of result, 31 s a round. */
constexpr rtype::master::CasinoSettings DEALER_TEST_SETTINGS{
    .rooms = 2,
    .seats = 8,
    .durations = {.betting = std::chrono::milliseconds{20'000},
                  .spinning = std::chrono::milliseconds{6'000},
                  .result = std::chrono::milliseconds{5'000}}};
constexpr std::chrono::milliseconds DEALER_TEST_CYCLE{31'000};
constexpr std::size_t DEALER_TEST_ROOMS = 2;

/** @brief The round the tests bet on, its start, and when its bets close. */
constexpr std::uint64_t BET_ROUND = 58'000'000;
constexpr rtype::network::MillisecondDate BET_ROUND_START{
    DEALER_TEST_CYCLE * static_cast<std::int64_t>(BET_ROUND)};
constexpr rtype::network::MillisecondDate BET_ROUND_CLOSE{
    BET_ROUND_START + std::chrono::milliseconds{20'000}};
constexpr std::chrono::milliseconds ONE_TICK{1};
constexpr std::chrono::milliseconds ONE_PASS{250};
constexpr std::uint8_t BET_ROOM = 1;

/** @brief The bets of ACE: 10 chips on 19 and 25 on red, 35 in all, out of
 * the 500 a wallet opens with. */
constexpr std::uint8_t STRAIGHT_ON = 19;
constexpr std::uint32_t STRAIGHT_STAKE = 10;
constexpr std::uint32_t RED_STAKE = 25;
constexpr std::uint32_t CHIPS_AT_OPENING = 500;
constexpr std::uint32_t CHIPS_ONCE_STAKED = 465;

/** @brief Numbers the draw gives: 19, red, which pays both bets, 36 to 1 on
 * the straight and 2 to 1 on red; 0, which pays neither. */
constexpr std::uint8_t RED_NINETEEN = 19;
constexpr std::uint8_t ZERO = 0;
constexpr std::uint32_t CHIPS_ONCE_BOTH_WON = 465 + (10 * 36) + (25 * 2);

/** @brief A second account of the tests. */
constexpr std::string_view SECOND_TEST_NICKNAME = "R9";
