#pragma once

#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include "BotLaunchConstants.hpp"
#include "ClientConstants.hpp"
#include "LaunchArgumentsConstants.hpp"

namespace rtype::tools::testing {

/** @brief Four full games of four players: the squad of a launch that names
 * no count. */
constexpr std::size_t FOUR_FULL_GAMES = 16;

/** @brief The 32 seats of a server of eight games, plus one game it
 * refuses. */
constexpr std::size_t LARGEST_SQUAD_SIZE = 36;

constexpr std::array<std::string_view, 2> NO_BOT{BOTS_OPTION, "0"};
constexpr std::array<std::string_view, 2> ONE_BOT{BOTS_OPTION, "1"};
constexpr std::array<std::string_view, 2> LARGEST_SQUAD{BOTS_OPTION, "36"};
constexpr std::array<std::string_view, 2> ONE_BOT_TOO_MANY{BOTS_OPTION, "37"};
constexpr std::array<std::string_view, 2> NO_SECOND{SECONDS_OPTION, "0"};

/** @brief A run shorter than the one of a launch that names no duration. */
constexpr std::chrono::seconds TWO_SECONDS{2};
constexpr std::array<std::string_view, 2> A_RUN_OF_TWO_SECONDS{SECONDS_OPTION,
                                                               "2"};

/** @brief A seed other than the one of a launch that names no seed. */
constexpr std::uint32_t ANOTHER_SEED_VALUE = 42;
constexpr std::array<std::string_view, 2> ANOTHER_SEED{SEED_OPTION, "42"};

constexpr std::string_view ANOTHER_SERVER_ADDRESS = "192.168.1.20";
constexpr std::uint16_t ANOTHER_SERVER_PORT = 5000;
constexpr std::array<std::string_view, 4> ANOTHER_SERVER{
    client::ADDRESS_OPTION, ANOTHER_SERVER_ADDRESS, engine::PORT_OPTION,
    "5000"};

constexpr std::array<std::string_view, 1> WITH_RULES{RULES_OPTION};

}  // namespace rtype::tools::testing
