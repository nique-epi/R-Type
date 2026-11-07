#include "MessageDispatcherFixtures.hpp"
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include "ConnectionAccepted.hpp"
#include "Endpoint.hpp"
#include "EntitySide.hpp"
#include "EntityTypeEntry.hpp"
#include "MessageDispatcher.hpp"
#include "MessageFrame.hpp"
#include "MessageOrigin.hpp"
#include "MessagePayload.hpp"
#include "MessageType.hpp"
#include "NetworkConstants.hpp"
#include "ProtocolFixtures.hpp"
#include "ProtocolTestConstants.hpp"
#include "ReadyState.hpp"

namespace rtype::network::testing {

MessageOrigin someOrigin() {
  return MessageOrigin{
      .sender = Endpoint{.address = std::string{LOOPBACK_IPV4_ADDRESS},
                         .port = SOME_SENDER_PORT},
      .header = someSessionHeader()};
}

std::vector<std::byte> acceptanceOfPlayerOne() {
  return encodeMessage(ConnectionAccepted{
      .playerNumber = 1,
      .types = {EntityTypeEntry{.side = EntitySide::Enemy, .name = "a"}}});
}

MessageFrame frameOf(MessageType type, const std::vector<std::byte>& payload) {
  return MessageFrame{.type = static_cast<std::uint8_t>(type),
                      .payload = payload};
}

MessageFrame frameOf(std::uint8_t type, const std::vector<std::byte>& payload) {
  return MessageFrame{.type = type, .payload = payload};
}

void recordReadyStates(MessageDispatcher& dispatcher,
                       ReadyStateRecord& record) {
  dispatcher.registerHandler<ReadyState>(
      [&record](const MessageOrigin& origin, const ReadyState& message) {
        record.origins.push_back(origin);
        record.messages.push_back(message);
      });
}

std::string someSenderText() {
  return "from " + std::string{LOOPBACK_IPV4_ADDRESS} + " port " +
         std::to_string(SOME_SENDER_PORT);
}

}  // namespace rtype::network::testing
