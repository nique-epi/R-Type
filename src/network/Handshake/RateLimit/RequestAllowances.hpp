#pragma once

#include <chrono>
#include <span>
#include <vector>
#include "AllowanceSlot.hpp"
#include "AllowanceVerdict.hpp"
#include "Endpoint.hpp"
#include "RateLimit.hpp"
#include "SipHash.hpp"

namespace rtype::network {

/**
 * @brief The allowances of the senders of connection requests, one per
 * address and port, in a table of fixed size: forged addresses cannot make it
 * grow.
 *
 * The table has REQUEST_ALLOWANCE_ROWS rows of REQUEST_ALLOWANCE_SLOTS_PER_ROW
 * slots. A sender always goes to the row its SipHash picks, under a key nobody
 * else knows, so nobody can aim at the row of another sender. A new sender
 * takes an empty slot of its row, else the slot seen longest ago among those
 * not banned, else the slot whose ban ends first: forged addresses replace one
 * another, never a banned sender while its row holds one that is not. A sender
 * replaced that way starts again with a whole burst. Not thread-safe.
 */
class RequestAllowances {
 public:
  /** @brief Draws its key with randomSipHashKey(). */
  explicit RequestAllowances(const RateLimit& limit);

  /** @param key The key the rows are picked with. */
  RequestAllowances(const RateLimit& limit, const SipHashKey& key);

  /**
   * @brief Spends one datagram of @p sender at @p now (see spendAllowance()).
   * @p now never goes back from one call to the next.
   */
  [[nodiscard]] AllowanceVerdict spend(const Endpoint& sender,
                                       std::chrono::nanoseconds now);

 private:
  [[nodiscard]] AllowanceSlot& slotOf(const Endpoint& sender,
                                      std::chrono::nanoseconds now);
  [[nodiscard]] std::span<AllowanceSlot> rowOf(const Endpoint& sender);

  RateLimit limit_;
  SipHashKey key_;
  std::vector<AllowanceSlot> slots_;
};

}  // namespace rtype::network
