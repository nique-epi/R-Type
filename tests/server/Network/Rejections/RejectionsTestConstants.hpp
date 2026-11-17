#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>
#include "EventField.hpp"

/** @brief The address every sender of the ledger tests sends from. */
constexpr std::string_view REJECTED_ADDRESS = "198.51.100.7";

/** @brief Another address, for a sender on the port of another one. */
constexpr std::string_view OTHER_REJECTED_ADDRESS = "198.51.100.8";

/**
 * @brief The port of the first sender of the ledger tests; the next ones
 * follow it.
 */
constexpr std::uint16_t FIRST_REJECTED_PORT = 50000;

/** @brief How many times the forgotten sender is rejected once it is back. */
constexpr std::size_t RETURNING_SENDER_REJECTIONS = 3;

/** @brief The address of the sender the lines of the report name. */
constexpr std::string_view LINE_SENDER_ADDRESS = "203.0.113.7";

/** @brief The port of the sender the lines of the report name. */
constexpr std::uint16_t LINE_SENDER_PORT = 40000;

/** @brief How many datagrams the report of the line tests says were rejected
 * since the start. */
constexpr std::size_t REJECTED_SINCE_START = 5;

/** @brief How many senders that report does not name. */
constexpr std::size_t UNNAMED_SENDERS = 2;

/** @brief How many senders it says were forgotten. */
constexpr std::size_t FORGOTTEN_SENDERS = 1;

/** @brief How many lines a report of one sender holds: its causes, then the
 * sender. */
constexpr std::size_t ONE_SENDER_REPORT_LINES = 2;

/** @brief What every line of the rejection report starts with. */
constexpr std::string_view REPORT_LINE_TEXT = "rejected ";

/**
 * @brief The lines of a report of 1 stranger and 3 wrong tokens in the last
 * 10 s, 5 since the start, the wrong tokens from LINE_SENDER_ADDRESS port
 * LINE_SENDER_PORT, with 2 more senders and 1 forgotten.
 */
constexpr std::string_view CAUSES_LINE =
    "rejected 4 datagrams in the last 10 s (5 since the start): 1 stranger, 3 "
    "wrong session token";
constexpr std::string_view SENDER_LINE =
    "rejected 3 datagrams from 203.0.113.7 port 40000 in the last 10 s: 3 "
    "wrong session token";
constexpr std::string_view OTHER_SENDERS_LINE =
    "rejected datagrams from 2 more senders in the last 10 s, besides 1 "
    "forgotten to keep at most 64";

/** @brief The names of the events of a report: its causes, a sender it names,
 * the senders it does not name. */
constexpr std::string_view CAUSES_EVENT_NAME = "datagrams.rejected";
constexpr std::string_view SENDER_EVENT_NAME = "datagrams.rejected_from";
constexpr std::string_view OTHER_SENDERS_EVENT_NAME =
    "datagrams.rejected_elsewhere";

/** @brief The values the events of that report hold: 10 s, 4 datagrams, 1 and
 * 3 of a cause, 5 since the start, the address and port of the sender, 2
 * unnamed senders. */
constexpr rtype::logging::EventFieldValue REPORT_LENGTH_FIELD{std::int64_t{10}};
constexpr rtype::logging::EventFieldValue CAUSES_TOTAL_FIELD{std::int64_t{4}};
constexpr rtype::logging::EventFieldValue ONE_FIELD{std::int64_t{1}};
constexpr rtype::logging::EventFieldValue THREE_FIELD{std::int64_t{3}};
constexpr rtype::logging::EventFieldValue SINCE_START_FIELD_VALUE{
    std::int64_t{5}};
constexpr rtype::logging::EventFieldValue SENDER_ADDRESS_FIELD{
    std::string_view{"203.0.113.7"}};
constexpr rtype::logging::EventFieldValue SENDER_PORT_FIELD{
    std::int64_t{40000}};
constexpr rtype::logging::EventFieldValue UNNAMED_SENDERS_FIELD{
    std::int64_t{2}};
