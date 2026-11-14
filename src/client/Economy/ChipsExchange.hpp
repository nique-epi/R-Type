#pragma once

#include <cstdint>
#include "Exchange.hpp"
#include "Purse.hpp"

namespace rtype::client {

/** @returns The change of @p amount kopecks of @p purse into as many chips:
 * the kopecks taken and the chips added, or the purse as it was and the first
 * refusal that holds: the amount is none, it is more than the kopecks, the
 * chips would pass MAXIMUM_CHIPS. An exchange is whole or not at all. */
[[nodiscard]] Exchange buyChips(const Purse& purse, std::uint32_t amount);

/** @returns The change of @p amount chips of @p purse back into as many
 * kopecks: the chips taken and the kopecks added, or the purse as it was and
 * the first refusal that holds: the amount is none, it is more than the chips,
 * the kopecks would pass MAXIMUM_KOPECKS. An exchange is whole or not at all.
 */
[[nodiscard]] Exchange cashOutChips(const Purse& purse, std::uint32_t amount);

}  // namespace rtype::client
