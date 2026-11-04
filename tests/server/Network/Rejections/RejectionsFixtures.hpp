#pragma once

#include <cstddef>
#include <initializer_list>
#include "Endpoint.hpp"
#include "RejectionCause.hpp"
#include "RejectionCounts.hpp"
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

/** @returns Counts holding one for each cause in @p causes. */
[[nodiscard]] rtype::server::RejectionCounts countsOf(
    std::initializer_list<rtype::server::RejectionCause> causes);
