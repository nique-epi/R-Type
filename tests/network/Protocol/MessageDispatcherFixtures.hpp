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

/** @returns A sender on the loopback address and a header of a session. */
MessageOrigin someOrigin();

/** @returns A frame holding @p payload as a message of type @p type. */
MessageFrame frameOf(MessageType type, const std::vector<std::byte>& payload);

/** @returns A frame holding @p payload under the type byte @p type, which may
 * be a type no message has. */
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
