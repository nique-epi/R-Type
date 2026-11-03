#include "RequestAllowances.hpp"
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <optional>
#include <span>
#include <utility>
#include "AllowanceSlot.hpp"
#include "AllowanceSpending.hpp"
#include "AllowanceVerdict.hpp"
#include "ByteWriter.hpp"
#include "Endpoint.hpp"
#include "EndpointBytes.hpp"
#include "RateLimit.hpp"
#include "RateLimitConstants.hpp"
#include "SenderAllowance.hpp"
#include "SipHash.hpp"

namespace rtype::network {

namespace {

AllowanceSlot* findHeld(std::span<AllowanceSlot> row, const Endpoint& sender) {
  const auto held =
      std::ranges::find_if(row, [&sender](const AllowanceSlot& slot) {
        return slot.lastSeen.has_value() && slot.sender == sender;
      });
  return held == row.end() ? nullptr : &*held;
}

/**
 * @returns How a slot ranks to be taken by a new sender at @p now, the lowest
 * first: the slots not banned before the banned ones, the first by when they
 * were last seen, the others by the end of their ban.
 */
std::pair<bool, std::chrono::nanoseconds> rankToReuse(
    const AllowanceSlot& slot, std::chrono::nanoseconds now) {
  if (isBannedAt(slot.allowance, now)) {
    return {true, slot.allowance.bannedUntil.value_or(now)};
  }
  return {false, slot.lastSeen.value_or(now)};
}

AllowanceSlot& slotToReuse(std::span<AllowanceSlot> row,
                           std::chrono::nanoseconds now) {
  const auto empty = std::ranges::find_if(row, [](const AllowanceSlot& slot) {
    return !slot.lastSeen.has_value();
  });
  if (empty != row.end()) {
    return *empty;
  }
  return *std::ranges::min_element(row, {}, [now](const AllowanceSlot& slot) {
    return rankToReuse(slot, now);
  });
}

AllowanceSlot& claim(AllowanceSlot& slot, const Endpoint& sender) {
  slot = AllowanceSlot{.sender = sender,
                       .lastSeen = std::nullopt,
                       .allowance = SenderAllowance{}};
  return slot;
}

}  // namespace

RequestAllowances::RequestAllowances(const RateLimit& limit)
    : RequestAllowances(limit, randomSipHashKey()) {}

RequestAllowances::RequestAllowances(const RateLimit& limit,
                                     const SipHashKey& key)
    : limit_(limit),
      key_(key),
      slots_(REQUEST_ALLOWANCE_ROWS * REQUEST_ALLOWANCE_SLOTS_PER_ROW) {}

AllowanceVerdict RequestAllowances::spend(const Endpoint& sender,
                                          std::chrono::nanoseconds now) {
  AllowanceSlot& slot = slotOf(sender, now);
  slot.lastSeen = now;
  return spendAllowance(slot.allowance, limit_, now);
}

AllowanceSlot& RequestAllowances::slotOf(const Endpoint& sender,
                                         std::chrono::nanoseconds now) {
  const std::span<AllowanceSlot> row = rowOf(sender);
  AllowanceSlot* const held = findHeld(row, sender);
  return held != nullptr ? *held : claim(slotToReuse(row, now), sender);
}

std::span<AllowanceSlot> RequestAllowances::rowOf(const Endpoint& sender) {
  ByteWriter message;
  writeEndpoint(message, sender);
  const auto row = static_cast<std::size_t>(sipHash(key_, message.bytes()) %
                                            REQUEST_ALLOWANCE_ROWS);
  return std::span{slots_}.subspan(row * REQUEST_ALLOWANCE_SLOTS_PER_ROW,
                                   REQUEST_ALLOWANCE_SLOTS_PER_ROW);
}

}  // namespace rtype::network
