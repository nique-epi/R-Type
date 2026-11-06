#pragma once

#include <chrono>
#include <cstdint>
#include "CookieWindows.hpp"
#include "MasterMessageConstants.hpp"
#include "ServerStatusRuleConstants.hpp"

namespace rtype::master {

/**
 * @brief How many servers one address may create in CREATION_WINDOW: a host
 * creates one or two, so a script that creates more is refused until the hour
 * has passed.
 */
constexpr std::int64_t MAXIMUM_CREATIONS_PER_ADDRESS = 5;
constexpr std::chrono::seconds CREATION_WINDOW = std::chrono::hours{1};

/**
 * @brief The windows of the token of a probe: one heartbeat interval each, and
 * as many accepted as a server may leave probes unanswered, so a token lives
 * as long as PROBE_ANSWER_SILENCE.
 */
constexpr network::CookieWindows PROBE_TOKEN_WINDOWS{
    .duration = network::MASTER_HEARTBEAT_INTERVAL,
    .accepted = UNANSWERED_PROBES_BEFORE_UNREACHABLE};

}  // namespace rtype::master
