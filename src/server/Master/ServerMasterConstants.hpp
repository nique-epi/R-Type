#pragma once

#include <chrono>
#include <string_view>

namespace rtype::server {

/**
 * @brief How long the server waits before it registers again after a refusal
 * or a master that does not answer. The least time between two heartbeats,
 * network::MINIMUM_NOTICE_GAP, and how often it sends one when nothing
 * changes, network::MASTER_HEARTBEAT_INTERVAL, are read by the master too.
 */
constexpr std::chrono::milliseconds REGISTRATION_RETRY{5'000};

/**
 * @brief The statuses after which the server registers again: the master does
 * not know its key, or does not know it registered.
 */
constexpr int UNKNOWN_KEY_STATUS = 401;
constexpr int UNREGISTERED_SERVER_STATUS = 409;

/** @brief Why a datagram of the queue of probes is dropped, as its lines say.
 */
constexpr std::string_view NO_MASTER_PROBE_DROP_REASON =
    "it holds no probe of the master";
constexpr std::string_view WRONG_PROBE_SECRET_DROP_REASON =
    "its probe carries another secret than the one this server registered with";

}  // namespace rtype::server
