#pragma once

#include <cstdint>
#include <optional>
#include <stop_token>
#include "LatestValue.hpp"
#include "MasterCallsConstants.hpp"
#include "NetworkException.hpp"

namespace rtype::client {

/** @returns What @p call reads, or nothing when the master does not answer,
 * refuses, or answers what cannot be read. */
template <typename Call>
auto readingOrNothing(Call call) -> std::optional<decltype(call())> {
  try {
    return call();
  } catch (const network::NetworkException&) {
    return std::nullopt;
  }
}

/**
 * @returns Why @p call failed, nothing when it went through: @p refusedOf
 * reads a refusal of the master, and the other failures are
 * Refusal::MasterUnreachable when no master answered, Refusal::MasterRefused
 * for an answer the client cannot read.
 */
template <typename Refusal, typename Call, typename RefusedOf>
std::optional<Refusal> masterRefusalOf(Call call, RefusedOf refusedOf) {
  try {
    call();
  } catch (const network::MasterRefusedException& refused) {
    return refusedOf(refused);
  } catch (const network::MasterUnreachableException&) {
    return Refusal::MasterUnreachable;
  } catch (const network::NetworkException&) {
    return Refusal::MasterRefused;
  }
  return std::nullopt;
}

/** @brief The loop of the thread of a link to the master: sleeps until
 * @p asks holds a newer value, MASTER_ASK_WAIT at most, and hands the newest
 * to @p serve, until @p stop is requested. */
template <typename Asks, typename Serve>
void serveAsksUntilStopped(const std::stop_token& stop,
                           const concurrency::LatestValue<Asks>& asks,
                           Serve serve) {
  std::uint64_t seen = 0;
  while (!stop.stop_requested()) {
    seen = asks.waitForNewer(seen, MASTER_ASK_WAIT, stop);
    if (const std::optional<Asks> newest = asks.value()) {
      serve(*newest);
    }
  }
}

}  // namespace rtype::client
