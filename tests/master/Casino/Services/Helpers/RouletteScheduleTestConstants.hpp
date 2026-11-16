#pragma once

#include <chrono>
#include <cstdint>
#include "MillisecondDate.hpp"
#include "RouletteDurations.hpp"

/** @brief The phases of the rounds of the tests: 20 s of bets, 6 s of spin,
 * 5 s of result, 31 s a round. */
constexpr rtype::master::RouletteDurations TEST_DURATIONS{
    .betting = std::chrono::milliseconds{20'000},
    .spinning = std::chrono::milliseconds{6'000},
    .result = std::chrono::milliseconds{5'000}};
constexpr std::chrono::milliseconds TEST_CYCLE{31'000};

/** @brief A round of the tests, which starts at TEST_ROUND rounds of 31 s
 * after 1970, and where its phases end, counted from its start. */
constexpr std::uint64_t TEST_ROUND = 57'000'000;
constexpr rtype::master::MillisecondDate TEST_ROUND_START{
    TEST_CYCLE * static_cast<std::int64_t>(TEST_ROUND)};
constexpr std::chrono::milliseconds BETS_CLOSE_AFTER{20'000};
constexpr std::chrono::milliseconds RESULT_SHOWS_AFTER{26'000};
constexpr std::chrono::milliseconds ONE_MILLISECOND{1};
