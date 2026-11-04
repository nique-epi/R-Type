#pragma once

#include <array>
#include <cstddef>
#include "RejectionCause.hpp"
#include "RejectionsConstants.hpp"

namespace rtype::server {

/** @brief One count for each RejectionCause, in the order they are declared. */
using RejectionCounts = std::array<std::size_t, REJECTION_CAUSE_COUNT>;

/** @returns The count of @p cause in @p counts. */
[[nodiscard]] std::size_t countOf(const RejectionCounts& counts,
                                  RejectionCause cause);

/** @brief Adds one to the count of @p cause in @p counts. */
void addOneTo(RejectionCounts& counts, RejectionCause cause);

/** @returns The counts of every cause in @p counts, added up. */
[[nodiscard]] std::size_t totalOf(const RejectionCounts& counts);

}  // namespace rtype::server
