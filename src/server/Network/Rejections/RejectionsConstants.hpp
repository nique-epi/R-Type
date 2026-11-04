#pragma once

#include <array>
#include <cstddef>
#include <string_view>
#include "RejectionCause.hpp"

namespace rtype::server {

/** @brief How many causes RejectionCause names. */
constexpr std::size_t REJECTION_CAUSE_COUNT =
    static_cast<std::size_t>(RejectionCause::RequestQueueFull) + 1;

/**
 * @brief The name of each RejectionCause in the lines of the rejection report,
 * in the order the causes are declared.
 */
constexpr std::array<std::string_view, REJECTION_CAUSE_COUNT>
    REJECTION_CAUSE_NAMES{"shorter than the header", "foreign header",
                          "message sizes",           "stranger",
                          "wrong session token",     "player queue full",
                          "request queue full"};

/**
 * @brief How many senders the RejectionLedger keeps between two reports; a new
 * sender then takes the place of the one rejected least recently.
 */
constexpr std::size_t REJECTED_SENDER_CAPACITY = 64;

/**
 * @brief How many senders a rejection report names, the most rejected first.
 */
constexpr std::size_t REPORTED_SENDER_COUNT = 5;

}  // namespace rtype::server
