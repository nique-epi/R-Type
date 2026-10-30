#include "SessionDatagram.hpp"
#include <cstddef>
#include <span>
#include "ConnectedPlayer.hpp"
#include "DatagramCodec.hpp"
#include "Endpoint.hpp"
#include "MessageType.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "PlayerRoster.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

network::PacketHeader nextSessionHeader(ConnectedPlayer& recipient) {
  network::PacketHeader header;
  header.sessionToken = recipient.sessionToken;
  header.sequence = recipient.nextSequence;
  ++recipient.nextSequence;
  recipient.sinceSent = engine::Duration::zero();
  return header;
}

network::PacketHeader sessionHeaderFor(PlayerRoster& roster,
                                       const network::Endpoint& recipient) {
  ConnectedPlayer* const player = roster.find(recipient);
  if (player == nullptr) {
    return network::PacketHeader{};
  }
  return nextSessionHeader(*player);
}

network::OutgoingDatagram sessionDatagram(ConnectedPlayer& recipient,
                                          network::MessageType type,
                                          std::span<const std::byte> payload) {
  return network::OutgoingDatagram{
      .destination = recipient.endpoint,
      .payload = network::DatagramCodec::encode(nextSessionHeader(recipient),
                                                type, payload)};
}

}  // namespace rtype::server
