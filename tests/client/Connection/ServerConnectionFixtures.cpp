#include "ServerConnectionFixtures.hpp"
#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "ActionSet.hpp"
#include "DatagramCodec.hpp"
#include "Endpoint.hpp"
#include "GameEnded.hpp"
#include "MessageFrame.hpp"
#include "MessagePayload.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "PlayerAction.hpp"
#include "PlayerInput.hpp"
#include "ProtocolConstants.hpp"
#include "ServerConnectionTestConstants.hpp"

namespace rtype::client::testing {

network::Endpoint server() {
  return network::Endpoint{.address = std::string{SERVER_ADDRESS},
                           .port = SERVER_PORT};
}

network::Endpoint otherSender() {
  return network::Endpoint{.address = std::string{SERVER_ADDRESS},
                           .port = OTHER_SENDER_PORT};
}

network::PlayerInput inputOf(const network::OutgoingDatagram& datagram) {
  const std::vector<network::MessageFrame> frames =
      network::DatagramCodec::splitMessages(datagram.payload);
  EXPECT_EQ(frames.size(), 1);
  return network::decodeMessage<network::PlayerInput>(frames.front().payload);
}

ActionSet upHeld() {
  ActionSet held;
  held.add(PlayerAction::Up);
  return held;
}

network::GameEnded endOfOnePilot() {
  return network::GameEnded{.players = {{.playerNumber = ACCEPTED_PLAYER_NUMBER,
                                         .score = PILOT_FINAL_SCORE}}};
}

network::PacketHeader guaranteedSessionHeader(std::uint16_t sequence) {
  network::PacketHeader header;
  header.sessionToken = SESSION_TOKEN;
  header.sequence = sequence;
  header.flags = network::GUARANTEED_FLAG;
  return header;
}

}  // namespace rtype::client::testing
