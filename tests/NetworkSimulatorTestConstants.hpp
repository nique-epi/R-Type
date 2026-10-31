#pragma once

#include <array>
#include <chrono>
#include <string_view>
#include "NetworkConditions.hpp"

/** @brief The options of the network simulator, spelled as the user chose
 * them. */
constexpr std::string_view NETWORK_OPTION_TEXT = "--network";
constexpr std::string_view LAG_OPTION_TEXT = "--lag";
constexpr std::string_view JITTER_OPTION_TEXT = "--jitter";
constexpr std::string_view LOSS_OPTION_TEXT = "--loss";
constexpr std::string_view DUPLICATES_OPTION_TEXT = "--duplicates";
constexpr std::string_view REORDER_OPTION_TEXT = "--reorder";
constexpr std::array SIMULATOR_OPTION_TEXTS{
    NETWORK_OPTION_TEXT, LAG_OPTION_TEXT,        JITTER_OPTION_TEXT,
    LOSS_OPTION_TEXT,    DUPLICATES_OPTION_TEXT, REORDER_OPTION_TEXT};

/**
 * @brief The scenarios as the user chose them: their names, then their ping
 * and jitter in milliseconds and the percent of the datagrams lost,
 * duplicated and reordered.
 */
constexpr std::string_view MOBILE_4G_NAME_TEXT = "4g";
constexpr std::string_view BUSY_WIFI_NAME_TEXT = "wifi-busy";
constexpr std::string_view BAD_NAME_TEXT = "bad";
constexpr rtype::network::NetworkConditions MOBILE_4G_OF_THE_TABLE{
    .lag = std::chrono::milliseconds{60},
    .jitter = std::chrono::milliseconds{20},
    .lossPercent = 1,
    .duplicatePercent = 0,
    .reorderPercent = 0};
constexpr rtype::network::NetworkConditions BUSY_WIFI_OF_THE_TABLE{
    .lag = std::chrono::milliseconds{30},
    .jitter = std::chrono::milliseconds{60},
    .lossPercent = 3,
    .duplicatePercent = 1,
    .reorderPercent = 2};
constexpr rtype::network::NetworkConditions BAD_OF_THE_TABLE{
    .lag = std::chrono::milliseconds{300},
    .jitter = std::chrono::milliseconds{50},
    .lossPercent = 10,
    .duplicatePercent = 2,
    .reorderPercent = 5};

/** @brief A network with a lag of 200 ms and nothing else, and the half of
 * it each datagram waits, sent or received. */
constexpr rtype::network::NetworkConditions LAGGING_NETWORK{
    .lag = std::chrono::milliseconds{200}};
constexpr std::chrono::milliseconds HALF_OF_THE_LAG{100};
