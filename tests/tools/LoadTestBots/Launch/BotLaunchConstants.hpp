#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include "ProtocolConstants.hpp"

namespace rtype::tools {

/** @brief The name of the tool, as its usage and its errors print it. */
constexpr std::string_view BOTS_PROGRAM_NAME = "load_test_bots";

/** @brief The options of the tool, besides `-h`, `-p` and `--help`, which it
 * reads as the client does. */
constexpr std::string_view BOTS_OPTION = "--bots";
constexpr std::string_view SECONDS_OPTION = "--seconds";
constexpr std::string_view SEED_OPTION = "--seed";

/**
 * @brief The bots of a launch that names no count: four full games, so the
 * server runs several games at once with every seat taken.
 */
constexpr std::size_t FULL_GAMES_BY_DEFAULT = 4;
constexpr std::size_t DEFAULT_BOT_COUNT =
    FULL_GAMES_BY_DEFAULT * network::MAXIMUM_PLAYERS;

/**
 * @brief The most bots one launch runs: far more than the seats of one
 * supervisor, which refuses the bots it has no seat for, so a launch also
 * loads the admission of a full server. Each bot has a socket and a network
 * thread of its own, so the count is bounded.
 */
constexpr std::size_t MAXIMUM_BOT_COUNT = 300;

/** @brief The options that choose what flies the bots: the last one wins. */
constexpr std::string_view RULES_OPTION = "--rules";

/** @brief How long a launch that names no duration plays. */
constexpr std::chrono::seconds DEFAULT_RUN_DURATION{60};

/**
 * @brief The seed of a launch that names none, so each launch presses the
 * same keys. Bot n, from 0, draws from this seed plus n.
 */
constexpr std::uint32_t DEFAULT_BOT_SEED = 1;

}  // namespace rtype::tools
