#include "LookRelayFixture.hpp"
#include <cstddef>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerLooks.hpp"
#include "SentDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "ShipLook.hpp"
#include "ShipLookChoice.hpp"

namespace rtype::server::testing {

LookRelayFixture::LookRelayFixture()
    : relay_(roster_, [this](network::OutgoingDatagram datagram) {
        sent_.push_back(std::move(datagram));
      }) {}

ConnectedPlayer& LookRelayFixture::seat(std::size_t index) {
  return *roster_.add(playerEndpoint(index),
                      std::string{NICKNAME_PREFIX} + std::to_string(index),
                      SESSION_TOKEN + index);
}

void LookRelayFixture::leave(std::size_t index) {
  const std::optional<ConnectedPlayer> departed =
      roster_.remove(playerEndpoint(index));
  if (departed.has_value()) {
    relay_.onPlayerClosing(*departed);
  }
}

void LookRelayFixture::choose(std::size_t index, identifiers::ShipLook look) {
  relay_.onChoice(*roster_.find(playerEndpoint(index)),
                  network::ShipLookChoice{.look = look});
}

void LookRelayFixture::arrive() { relay_.onPlayerJoined(); }

void LookRelayFixture::runTicks(std::size_t count) {
  for (std::size_t tick = 0; tick < count; ++tick) {
    relay_.onTick();
  }
}

std::vector<network::PlayerLooks> LookRelayFixture::looksTo(
    std::size_t index) const {
  std::vector<network::PlayerLooks> looks;
  for (const network::OutgoingDatagram& datagram :
       sentTo(sent_, playerEndpoint(index))) {
    for (const network::PlayerLooks& message :
         messagesIn<network::PlayerLooks>(datagram)) {
      looks.push_back(message);
    }
  }
  return looks;
}

const std::vector<network::OutgoingDatagram>& LookRelayFixture::sent() const {
  return sent_;
}

}  // namespace rtype::server::testing
