#include "MessageDispatcher.hpp"
#include <any>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include "MessageFrame.hpp"
#include "MessageOrigin.hpp"
#include "NetworkConstants.hpp"
#include "NetworkException.hpp"

namespace rtype::network {

MessageDispatcher::MessageDispatcher()
    : rejectionLog_(std::string{NETWORK_LOGGER_NAME}, REJECTION_LINE_INTERVAL) {
}

void MessageDispatcher::dispatch(const MessageOrigin& origin,
                                 std::span<const MessageFrame> messages) {
  for (const MessageFrame& message : messages) {
    dispatchOne(origin, message);
  }
}

std::size_t MessageDispatcher::skippedMessageCount() const {
  return skippedMessageCount_;
}

std::size_t MessageDispatcher::malformedMessageCount() const {
  return malformedMessageCount_;
}

void MessageDispatcher::dispatchOne(const MessageOrigin& origin,
                                    const MessageFrame& message) {
  const Route& route = routes_.at(message.type);
  if (!route.decode) {
    skipUnhandled(origin, message.type);
    return;
  }
  std::any decoded;
  try {
    decoded = route.decode(message.payload);
  } catch (const NetworkException& error) {
    dropMalformed(origin, message.type, error);
    return;
  }
  route.handle(origin, decoded);
}

void MessageDispatcher::skipUnhandled(const MessageOrigin& origin,
                                      std::uint8_t type) {
  ++skippedMessageCount_;
  rejectionLog_.debug(std::chrono::steady_clock::now().time_since_epoch(),
                      "skipped a message of type ",
                      static_cast<unsigned int>(type), " from ",
                      origin.sender.address, " port ", origin.sender.port,
                      ": no handler for that type");
}

void MessageDispatcher::dropMalformed(const MessageOrigin& origin,
                                      std::uint8_t type,
                                      const NetworkException& error) {
  ++malformedMessageCount_;
  rejectionLog_.debug(
      std::chrono::steady_clock::now().time_since_epoch(),
      "dropped a message of type ", static_cast<unsigned int>(type), " from ",
      origin.sender.address, " port ", origin.sender.port, ": ", error.what());
}

}  // namespace rtype::network
