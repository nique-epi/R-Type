#include "ChatRelayFixture.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include "ChatLine.hpp"
#include "ChatRelayTestConstants.hpp"
#include "ConnectedPlayer.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerChatLine.hpp"
#include "SentDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"

namespace rtype::server::testing {

ChatRelayFixture::ChatRelayFixture()
    : relay_(roster_, [this](network::OutgoingDatagram datagram) {
        sent_.push_back(std::move(datagram));
      }) {}

ConnectedPlayer& ChatRelayFixture::seat(std::size_t index) {
  return *roster_.add(playerEndpoint(index),
                      std::string{NICKNAME_PREFIX} + std::to_string(index),
                      SESSION_TOKEN + index);
}

void ChatRelayFixture::leave(std::size_t index) {
  const std::optional<ConnectedPlayer> departed =
      roster_.remove(playerEndpoint(index));
  if (departed.has_value()) {
    relay_.onPlayerClosing(*departed);
  }
}

void ChatRelayFixture::say(std::size_t index, std::uint16_t lineNumber) {
  relay_.onLine(*roster_.find(playerEndpoint(index)),
                network::ChatLine{.lineNumber = lineNumber,
                                  .text = std::string{SOME_TEXT}});
}

void ChatRelayFixture::runTicks(std::size_t count) {
  for (std::size_t tick = 0; tick < count; ++tick) {
    relay_.onTick();
  }
}

std::vector<network::PlayerChatLine> ChatRelayFixture::linesTo(
    std::size_t index) const {
  std::vector<network::PlayerChatLine> lines;
  for (const network::OutgoingDatagram& datagram :
       sentTo(sent_, playerEndpoint(index))) {
    for (const network::PlayerChatLine& line :
         messagesIn<network::PlayerChatLine>(datagram)) {
      lines.push_back(line);
    }
  }
  return lines;
}

const std::vector<network::OutgoingDatagram>& ChatRelayFixture::sent() const {
  return sent_;
}

}  // namespace rtype::server::testing
