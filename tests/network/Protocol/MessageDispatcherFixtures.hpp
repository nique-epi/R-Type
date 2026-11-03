#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include "MessageDispatcher.hpp"
#include "MessageFrame.hpp"
#include "MessageOrigin.hpp"
#include "MessageType.hpp"
#include "ReadyState.hpp"

namespace rtype::network::testing {

/**
 * @returns The origin of the messages of the tests: SOME_SENDER_PORT on the
 * loopback address, under someSessionHeader().
 */
MessageOrigin someOrigin();

/** @returns A message of type @p type carrying @p payload. */
MessageFrame frameOf(MessageType type, const std::vector<std::byte>& payload);

/** @returns A message of the type byte @p type carrying @p payload. */
MessageFrame frameOf(std::uint8_t type, const std::vector<std::byte>& payload);

/**
 * The ReadyState messages a dispatcher handed over, with their origins.
 */
struct ReadyStateRecord {
  std::vector<ReadyState> messages;
  std::vector<MessageOrigin> origins;
};

/**
 * Registers the ReadyState handler of @p dispatcher, which appends each
 * message and its origin to @p record; @p record must outlive the dispatches.
 */
void recordReadyStates(MessageDispatcher& dispatcher, ReadyStateRecord& record);

}  // namespace rtype::network::testing
