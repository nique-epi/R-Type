#pragma once

#include <array>
#include <cstddef>
#include <string_view>
#include "RejectionCause.hpp"

namespace rtype::server {

/** @brief How many causes RejectionCause names. */
constexpr std::size_t REJECTION_CAUSE_COUNT =
    static_cast<std::size_t>(RejectionCause::RefusedConnection) + 1;

/**
 * @brief The name of each RejectionCause in the lines of the rejection report,
 * in the order the causes are declared.
 */
constexpr std::array<std::string_view, REJECTION_CAUSE_COUNT>
    REJECTION_CAUSE_NAMES{"shorter than the header",
                          "foreign header",
                          "message sizes",
                          "stranger",
                          "wrong session token",
                          "over rate limit",
                          "newly banned",
                          "banned",
                          "player queue full",
                          "request queue full",
                          "information queue full",
                          "probe queue full",
                          "refused connection"};

/**
 * @brief The key of each RejectionCause in the events of the rejection report,
 * the field of its count, in the order the causes are declared: a tool reads
 * them, so they stay apart from the words of the lines.
 */
constexpr std::array<std::string_view, REJECTION_CAUSE_COUNT>
    REJECTION_CAUSE_KEYS{"shorter_than_header",    "foreign_header",
                         "message_sizes",          "stranger",
                         "wrong_session_token",    "over_rate_limit",
                         "newly_banned",           "banned",
                         "player_queue_full",      "request_queue_full",
                         "information_queue_full", "probe_queue_full",
                         "refused_connection"};

/**
 * @brief How many senders the RejectionLedger keeps between two reports; a new
 * sender then takes the place of the one rejected least recently.
 */
constexpr std::size_t REJECTED_SENDER_CAPACITY = 64;

/**
 * @brief How many senders a rejection report names, the most rejected first.
 */
constexpr std::size_t REPORTED_SENDER_COUNT = 5;

/**
 * @brief The events of a rejection report: the count of each cause, one
 * sender the report names, and the senders it does not name.
 */
constexpr std::string_view REJECTED_DATAGRAMS_EVENT = "datagrams.rejected";
constexpr std::string_view REJECTED_FROM_SENDER_EVENT =
    "datagrams.rejected_from";
constexpr std::string_view REJECTED_FROM_OTHER_SENDERS_EVENT =
    "datagrams.rejected_elsewhere";

/**
 * @brief The fields of those events besides the counts of the causes: how
 * long a report lasts, how many datagrams it counts, how many since the
 * start, and how many senders it does not name or forgot.
 */
constexpr std::string_view SECONDS_FIELD = "seconds";
constexpr std::string_view TOTAL_FIELD = "total";
constexpr std::string_view SINCE_START_FIELD = "since_start";
constexpr std::string_view SENDERS_FIELD = "senders";
constexpr std::string_view FORGOTTEN_SENDERS_FIELD = "forgotten_senders";

}  // namespace rtype::server
