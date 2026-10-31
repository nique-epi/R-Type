#pragma once

#include <array>
#include <chrono>
#include <string_view>
#include "NetworkConditions.hpp"
#include "NetworkScenario.hpp"

namespace rtype::network {

/**
 * @brief A network that delays, loses, copies and reorders nothing: a program
 * given no simulator option opens its UDP socket as it is.
 */
constexpr NetworkConditions PERFECT_NETWORK{};

/** @brief Launch option followed by the name of a scenario of
 * NETWORK_SCENARIOS. */
constexpr std::string_view NETWORK_OPTION = "--network";

/** @brief Launch options followed by a lag or a jitter, in milliseconds. */
constexpr std::string_view LAG_OPTION = "--lag";
constexpr std::string_view JITTER_OPTION = "--jitter";

/** @brief Launch options followed by a share of the datagrams, in percent. */
constexpr std::string_view LOSS_OPTION = "--loss";
constexpr std::string_view DUPLICATES_OPTION = "--duplicates";
constexpr std::string_view REORDER_OPTION = "--reorder";

/**
 * @brief Longest lag or jitter the options take: ten seconds, far past any
 * playable ping. The bound only refuses a mistyped value.
 */
constexpr std::chrono::milliseconds MAXIMUM_SIMULATED_DELAY{10000};

/** @brief Largest share of the datagrams a defect may touch, in percent. */
constexpr int MAXIMUM_PERCENT = 100;

/**
 * @brief How much longer than the others a reordered datagram is held: more
 * than the 50 ms between two world states at the default rate, so the next
 * state arrives first.
 */
constexpr std::chrono::milliseconds REORDER_HOLD{75};

/** @brief A mobile 4G network: ping +60 ms, up to 20 ms more, 1 % lost. */
constexpr std::string_view MOBILE_4G_SCENARIO_NAME = "4g";
constexpr NetworkConditions MOBILE_4G_NETWORK{
    .lag = std::chrono::milliseconds{60},
    .jitter = std::chrono::milliseconds{20},
    .lossPercent = 1,
    .duplicatePercent = 0,
    .reorderPercent = 0};

/** @brief A busy Wi-Fi: little lag, a jitter of 60 ms, 3 % lost, 1 %
 * duplicated, 2 % reordered. */
constexpr std::string_view BUSY_WIFI_SCENARIO_NAME = "wifi-busy";
constexpr NetworkConditions BUSY_WIFI_NETWORK{
    .lag = std::chrono::milliseconds{30},
    .jitter = std::chrono::milliseconds{60},
    .lossPercent = 3,
    .duplicatePercent = 1,
    .reorderPercent = 2};

/** @brief A bad network: ping +300 ms, up to 50 ms more, 10 % lost, 2 %
 * duplicated, 5 % reordered. */
constexpr std::string_view BAD_SCENARIO_NAME = "bad";
constexpr NetworkConditions BAD_NETWORK{.lag = std::chrono::milliseconds{300},
                                        .jitter = std::chrono::milliseconds{50},
                                        .lossPercent = 10,
                                        .duplicatePercent = 2,
                                        .reorderPercent = 5};

/** @brief Every scenario `--network` knows, in the order the usage lists
 * them. */
constexpr std::array NETWORK_SCENARIOS{
    NetworkScenario{.name = MOBILE_4G_SCENARIO_NAME,
                    .conditions = MOBILE_4G_NETWORK},
    NetworkScenario{.name = BUSY_WIFI_SCENARIO_NAME,
                    .conditions = BUSY_WIFI_NETWORK},
    NetworkScenario{.name = BAD_SCENARIO_NAME, .conditions = BAD_NETWORK},
};

/** @brief Title of the part of a usage that describes the simulator
 * options. */
constexpr std::string_view SIMULATOR_USAGE_TITLE = "Network simulator options:";

/**
 * @brief What starts the line of an option, and the line that describes it,
 * in that part of a usage: the same as engine::USAGE_OPTION_INDENT and
 * engine::USAGE_DESCRIPTION_INDENT, which the network module cannot include.
 */
constexpr std::string_view SIMULATOR_USAGE_OPTION_INDENT = "  ";
constexpr std::string_view SIMULATOR_USAGE_DESCRIPTION_INDENT = "      ";

}  // namespace rtype::network
