#pragma once

#include <cstddef>
#include <initializer_list>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include "Endpoint.hpp"
#include "EventField.hpp"
#include "RejectionCause.hpp"
#include "RejectionCounts.hpp"
#include "RejectionEvent.hpp"
#include "RejectionLedger.hpp"

/**
 * @returns The sender number @p index of the ledger tests: REJECTED_ADDRESS,
 * from port FIRST_REJECTED_PORT on.
 */
[[nodiscard]] rtype::network::Endpoint rejectedSender(std::size_t index);

/** @brief Records one Stranger from the sender number @p index. */
void recordStranger(rtype::server::RejectionLedger& ledger, std::size_t index);

/**
 * @brief Records one Stranger from each of the first @p count senders, the
 * sender number 0 first.
 */
void recordStrangers(rtype::server::RejectionLedger& ledger, std::size_t count);

/** @returns The count of @p cause in @p counts. */
[[nodiscard]] std::size_t countOf(const rtype::server::RejectionCounts& counts,
                                  rtype::server::RejectionCause cause);

/** @returns Counts holding one for each cause in @p causes. */
[[nodiscard]] rtype::server::RejectionCounts countsOf(
    std::initializer_list<rtype::server::RejectionCause> causes);

/** @returns The messages of @p events, in order. */
[[nodiscard]] std::vector<std::string> messagesOf(
    const std::vector<rtype::server::RejectionEvent>& events);

/** @returns The value of the field of @p event named @p name; nothing when it
 * has no such field. */
[[nodiscard]] std::optional<rtype::logging::EventFieldValue> fieldOf(
    const rtype::server::RejectionEvent& event, std::string_view name);
