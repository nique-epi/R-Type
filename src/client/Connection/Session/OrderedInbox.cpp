#include "OrderedInbox.hpp"
#include <cstdint>
#include <vector>
#include "EncodedMessage.hpp"
#include "MessageFrame.hpp"
#include "MessageOrigin.hpp"
#include "OrderedMessage.hpp"

namespace rtype::client {

OrderedInbox::OrderedInbox(network::MessageDispatcher& dispatcher)
    : dispatcher_(&dispatcher) {
  dispatcher.registerHandler<network::OrderedMessage>(
      [this](const network::MessageOrigin& origin,
             const network::OrderedMessage& envelope) {
        deliver(origin, envelope);
      });
}

void OrderedInbox::sessionOpened(
    [[maybe_unused]] const network::LobbyPlayer& ownPlayer,
    [[maybe_unused]] ISessionOutbox& outbox) {}

void OrderedInbox::sessionClosed() { receiver_.reset(); }

void OrderedInbox::sessionHeld([[maybe_unused]] engine::Duration elapsed) {}

void OrderedInbox::deliver(const network::MessageOrigin& origin,
                           const network::OrderedMessage& envelope) {
  const std::vector<network::EncodedMessage> ready =
      receiver_.receive(envelope);
  std::vector<network::MessageFrame> frames;
  frames.reserve(ready.size());
  for (const network::EncodedMessage& carried : ready) {
    frames.push_back(
        network::MessageFrame{.type = static_cast<std::uint8_t>(carried.type),
                              .payload = carried.payload});
  }
  dispatcher_->dispatch(origin, frames);
}

}  // namespace rtype::client
