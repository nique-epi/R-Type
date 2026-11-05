#pragma once

#include <chrono>

namespace rtype::server {

/**
 * @brief How often the server tells the master its state when nothing changes,
 * the least time between two heartbeats when games change, and how long it
 * waits before it registers again after a refusal or a master that does not
 * answer.
 */
constexpr std::chrono::milliseconds HEARTBEAT_INTERVAL{5'000};
constexpr std::chrono::milliseconds MINIMUM_NOTICE_GAP{1'000};
constexpr std::chrono::milliseconds REGISTRATION_RETRY{5'000};

/**
 * @brief The statuses after which the server registers again: the master does
 * not know its key, or does not know it registered.
 */
constexpr int UNKNOWN_KEY_STATUS = 401;
constexpr int UNREGISTERED_SERVER_STATUS = 409;

}  // namespace rtype::server
