#pragma once

#include <chrono>
#include <cstdint>

/** @brief The names of the probe modules the tests list after the modules of
 * the server. */
constexpr const char* PROBE_MODULE = "probe";
constexpr const char* PORT_PROBE_MODULE = "port probe";

/** @brief The text the failing stage work of the tests throws. */
constexpr const char* STAGE_FAILURE_TEXT = "a stage failed";

/** @brief The session token of the player the tests admit. */
constexpr std::uint64_t ADMITTED_TOKEN = 0xAB12;

/** @brief The seed of the games the tests start, and the name and the first
 * session token of the players they connect. */
constexpr std::uint32_t SOME_SEED = 7;
constexpr const char* PLAYER_NICKNAME = "ace";
constexpr std::uint64_t PLAYER_TOKEN_BASE = 0x1000;

/** @brief The simulation ticks that make two copies of a repeated message
 * 50 ms apart, with a tick to spare. */
constexpr int TICKS_FOR_ONE_MORE_COPY = 4;
constexpr int TICKS_FOR_THE_LAST_COPY = 8;

/** @brief A waves file that is not there. */
constexpr const char* MISSING_WAVES_FILE = "no/such/folder/waves.toml";

/** @brief How long a test waits for something a thread of the server does. */
constexpr std::chrono::seconds WAIT_FOR_THREADS{5};

/** @brief How many passes of the loop a test waits for. */
constexpr int PASSES_TO_WAIT_FOR = 2;
