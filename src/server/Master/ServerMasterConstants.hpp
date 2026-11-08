#pragma once

#include <chrono>
#include <cstddef>
#include <string_view>
#include "HttpStatusConstants.hpp"

namespace rtype::server {

/**
 * @brief The longest wait before the server registers again after the first
 * refusal, or the first time the master does not answer (see RetryBackoff).
 * The least time between two heartbeats, network::MINIMUM_NOTICE_GAP, and how
 * often it sends one when nothing changes, network::MASTER_HEARTBEAT_INTERVAL,
 * are read by the master too.
 */
constexpr std::chrono::milliseconds REGISTRATION_RETRY{5'000};

/**
 * @brief The longest wait between two registrations, however long the master
 * stays away. It bounds the wait only: a registration under way when the
 * master comes back first runs to its end, within the timeouts of
 * network::MasterClient.
 */
constexpr std::chrono::milliseconds MAXIMUM_REGISTRATION_RETRY{60'000};

/** @brief How much the longest wait grows after each failed registration. */
constexpr int RETRY_GROWTH = 2;

/**
 * @brief The statuses after which the server registers again: the master does
 * not know its key, or does not know it registered.
 */
constexpr int UNKNOWN_KEY_STATUS = network::UNAUTHORIZED_STATUS;
constexpr int UNREGISTERED_SERVER_STATUS = network::CONFLICT_STATUS;

/**
 * @brief The waiting results MasterLink sends after a heartbeat, the oldest
 * first; the others go after the next one. Provisional: a few milliseconds
 * each on a local network, so a backlog of days never holds the next heartbeat
 * back by more than a fraction of network::MINIMUM_NOTICE_GAP.
 */
constexpr std::size_t MAXIMUM_RESULTS_PER_ROUND = 16;

/**
 * @brief The status of a result the master refuses for good: a game it never
 * keeps, whatever the number of tries (not vanilla, or a member out of its
 * bounds). Every other refusal may not last, so the result waits.
 */
constexpr int FOR_GOOD_REFUSAL_STATUS = network::BAD_REQUEST_STATUS;

/** @brief Why a datagram of the queue of probes is dropped, as its lines say.
 */
constexpr std::string_view NO_MASTER_PROBE_DROP_REASON =
    "it holds no probe of the master";
constexpr std::string_view WRONG_PROBE_SECRET_DROP_REASON =
    "its probe carries another secret than the one this server registered with";

/** @brief The addresses public-address refuses, since no player reaches this
 * server at them: the start of every loopback address (127.0.0.0/8), and the
 * broadcast address. network::ANY_IPV4_ADDRESS is refused too. */
constexpr std::string_view LOOPBACK_IPV4_PREFIX = "127.";
constexpr std::string_view BROADCAST_IPV4_ADDRESS = "255.255.255.255";

/** @brief Why public-address refuses one of those addresses, after the
 * address. */
constexpr std::string_view UNREACHABLE_PUBLIC_ADDRESS_REASON =
    " is no address a player reaches this server at; leave public-address "
    "empty to let the master take the address it is called from";

}  // namespace rtype::server
