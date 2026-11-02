#pragma once

#include <chrono>
#include <cstddef>
#include <string_view>

namespace rtype::server {

/**
 * @brief Results waiting for the storage thread. Provisional: 8 games ending
 * together take 8 places, so 64 survives a disk stalled for many games. Above
 * it, the oldest is discarded and the journal says so.
 */
constexpr std::size_t WAITING_RESULTS_CAPACITY = 64;

/**
 * @brief Requests waiting for the storage thread. Provisional: one second of
 * every player of 8 games asking as often as they may. Above it, the oldest is
 * discarded and its client asks again.
 */
constexpr std::size_t WAITING_ASKS_CAPACITY = 64;

/** @brief How long the storage thread sleeps with nothing to do. Provisional:
 * an answer waits for it, on a screen a player opens by hand. */
constexpr std::chrono::milliseconds STORAGE_POLL_INTERVAL{20};

/** @brief Module name the storage writes its log lines under. */
constexpr std::string_view STORAGE_LOGGER_NAME = "Storage";

}  // namespace rtype::server
