#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include "ProtocolConstants.hpp"
#include "ServerConstants.hpp"

namespace rtype::tools {

/** @brief The name of the tool, as its usage and its errors print it. */
constexpr std::string_view BOTS_PROGRAM_NAME = "load_test_bots";

/** @brief The options of the tool, besides `-h`, `-p` and `--help`, which it
 * reads as the client does. */
constexpr std::string_view BOTS_OPTION = "--bots";
constexpr std::string_view SECONDS_OPTION = "--seconds";
constexpr std::string_view SEED_OPTION = "--seed";

/**
 * @brief The seats of a supervisor running MAXIMUM_RUNNING_GAMES games of
 * MAXIMUM_PLAYERS players: 32.
 */
constexpr std::size_t SERVER_SEATS =
    server::MAXIMUM_RUNNING_GAMES * network::MAXIMUM_PLAYERS;

/**
 * @brief The bots of a launch that names no count: four full games, so the
 * server runs several games at once with every seat taken.
 */
constexpr std::size_t FULL_GAMES_BY_DEFAULT = 4;
constexpr std::size_t DEFAULT_BOT_COUNT =
    FULL_GAMES_BY_DEFAULT * network::MAXIMUM_PLAYERS;

/**
 * @brief The most bots one launch runs: every seat of a server, plus one game
 * that the full server refuses, so its refusals show too. Each bot has a
 * socket and a network thread of its own, so the count is bounded.
 */
constexpr std::size_t MAXIMUM_BOT_COUNT =
    SERVER_SEATS + network::MAXIMUM_PLAYERS;

/** @brief How long a launch that names no duration plays. */
constexpr std::chrono::seconds DEFAULT_RUN_DURATION{60};

/**
 * @brief The seed of a launch that names none, so each launch presses the
 * same keys. Bot n, from 0, draws from this seed plus n.
 */
constexpr std::uint32_t DEFAULT_BOT_SEED = 1;

}  // namespace rtype::tools
