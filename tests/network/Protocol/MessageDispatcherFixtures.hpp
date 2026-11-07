#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include "MessageDispatcher.hpp"
#include "MessageFrame.hpp"
#include "MessageOrigin.hpp"
#include "MessagePayload.hpp"
#include "MessageType.hpp"
#include "NetworkException.hpp"
#include "ReadyState.hpp"

namespace rtype::network::testing {

/**
 * @returns The origin of the messages of the tests: SOME_SENDER_PORT on the
 * loopback address, under someSessionHeader().
 */
MessageOrigin someOrigin();

/** @returns The payload of a valid ConnectionAccepted: player 1, one enemy
 * type named a, and a gun that predicts nothing. */
std::vector<std::byte> acceptanceOfPlayerOne();

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

/**
 * Registers a handler for Message on @p dispatcher that throws
 * InvalidCoordinateException, a network error, whatever it gets.
 */
template <DecodableMessage Message>
void registerThrowingHandler(MessageDispatcher& dispatcher) {
  dispatcher.registerHandler<Message>([](const MessageOrigin&, const Message&) {
    throw InvalidCoordinateException();
  });
}

/**
 * @returns How a log line names the sender of someOrigin(): its address and
 * port.
 */
std::string someSenderText();

}  // namespace rtype::network::testing
