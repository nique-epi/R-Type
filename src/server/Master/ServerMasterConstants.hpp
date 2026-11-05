#pragma once

#include <chrono>

namespace rtype::server {

/**
 * @brief The least time between two heartbeats when games change, and how long
 * the server waits before it registers again after a refusal or a master that
 * does not answer. How often it sends one when nothing changes is
 * network::MASTER_HEARTBEAT_INTERVAL, which the master reads too.
 */
constexpr std::chrono::milliseconds MINIMUM_NOTICE_GAP{1'000};
constexpr std::chrono::milliseconds REGISTRATION_RETRY{5'000};

/**
 * @brief The statuses after which the server registers again: the master does
 * not know its key, or does not know it registered.
 */
constexpr int UNKNOWN_KEY_STATUS = 401;
constexpr int UNREGISTERED_SERVER_STATUS = 409;

}  // namespace rtype::server
