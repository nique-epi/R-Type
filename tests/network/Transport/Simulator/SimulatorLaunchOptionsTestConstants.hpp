#pragma once

#include <chrono>
#include <string_view>
#include "NetworkConditions.hpp"

/** @brief Arguments of the programs that are not simulator options. */
constexpr std::string_view PORT_OPTION_TEXT = "-p";
constexpr std::string_view PORT_ARGUMENT = "4243";
constexpr std::string_view NO_SOUND_OPTION_TEXT = "--no-sound";

/** @brief Values the tests give, and the conditions each makes. */
constexpr std::string_view LAG_ARGUMENT = "100";
constexpr rtype::network::NetworkConditions LAG_OF_LAG_ARGUMENT{
    .lag = std::chrono::milliseconds{100}};
constexpr std::string_view JITTER_ARGUMENT = "40";
constexpr rtype::network::NetworkConditions JITTER_OF_JITTER_ARGUMENT{
    .jitter = std::chrono::milliseconds{40}};
constexpr std::string_view LOSS_ARGUMENT = "3";
constexpr rtype::network::NetworkConditions LOSS_OF_LOSS_ARGUMENT{.lossPercent =
                                                                      3};
constexpr std::string_view DUPLICATES_ARGUMENT = "4";
constexpr rtype::network::NetworkConditions DUPLICATES_OF_DUPLICATES_ARGUMENT{
    .duplicatePercent = 4};
constexpr std::string_view REORDER_ARGUMENT = "6";
constexpr rtype::network::NetworkConditions REORDER_OF_REORDER_ARGUMENT{
    .reorderPercent = 6};

/** @brief BAD_OF_THE_TABLE with the loss of LOSS_ARGUMENT. */
constexpr rtype::network::NetworkConditions BAD_WITH_LOSS_OF_LOSS_ARGUMENT{
    .lag = std::chrono::milliseconds{300},
    .jitter = std::chrono::milliseconds{50},
    .lossPercent = 3,
    .duplicatePercent = 2,
    .reorderPercent = 5};

/** @brief The bounds: the longest delay, one millisecond more, a full share,
 * one percent more, and zero. */
constexpr std::string_view LONGEST_DELAY_ARGUMENT = "10000";
constexpr rtype::network::NetworkConditions LONGEST_LAG{
    .lag = std::chrono::milliseconds{10000}};
constexpr std::string_view TOO_LONG_DELAY_ARGUMENT = "10001";
constexpr std::string_view FULL_SHARE_ARGUMENT = "100";
constexpr rtype::network::NetworkConditions FULL_LOSS_OF_THE_OPTIONS{
    .lossPercent = 100};
constexpr std::string_view TOO_LARGE_SHARE_ARGUMENT = "101";
constexpr std::string_view ZERO_ARGUMENT = "0";

/** @brief Values no option takes. */
constexpr std::string_view UNKNOWN_SCENARIO_ARGUMENT = "5g";
constexpr std::string_view NEGATIVE_ARGUMENT = "-1";
constexpr std::string_view NOT_A_NUMBER_ARGUMENT = "fast";
constexpr std::string_view EMPTY_ARGUMENT{};
