#include "MessageDispatcherFixtures.hpp"
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include "Endpoint.hpp"
#include "MessageDispatcher.hpp"
#include "MessageFrame.hpp"
#include "MessageOrigin.hpp"
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
