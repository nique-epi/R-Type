#include "MessageDeliveryFixtures.hpp"
#include <cstddef>
#include "DatagramCodec.hpp"
#include "EncodedMessage.hpp"
#include "EntityDestroyed.hpp"
#include "MessageDeliveryTestConstants.hpp"
#include "MessagePayload.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "PlayerLeft.hpp"

namespace rtype::server::testing {

network::EncodedMessage playerLeftMessage() {
  return network::EncodedMessage{
      .type = network::PlayerLeft::TYPE,
      .payload = network::encodeMessage(SECOND_PLAYER_LEFT)};
}

network::EncodedMessage entityDestroyedMessage() {
  return network::EncodedMessage{
      .type = network::EntityDestroyed::TYPE,
      .payload = network::encodeMessage(MONSTER_KILLED)};
}

network::PacketHeader headerOf(const network::OutgoingDatagram& datagram) {
  return network::DatagramCodec::decodeHeader(datagram.payload);
}

std::size_t messageCountIn(const network::OutgoingDatagram& datagram) {
  return network::DatagramCodec::splitMessages(datagram.payload).size();
}

}  // namespace rtype::server::testing
