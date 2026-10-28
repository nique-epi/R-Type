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

/** @brief How long a test waits for something a thread of the server does. */
constexpr std::chrono::seconds WAIT_FOR_THREADS{5};

/** @brief How many passes of the loop a test waits for. */
constexpr int PASSES_TO_WAIT_FOR = 2;
