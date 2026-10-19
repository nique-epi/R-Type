#include "EntitySpawnSender.hpp"
#include "ConnectedPlayer.hpp"
#include "DatagramCodec.hpp"
#include "EntitySpawned.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"

namespace rtype::server {

void sendEntitySpawned(const SendDatagram& send, ConnectedPlayer& recipient,
                       const network::EntitySpawned& spawned) {
  network::PacketHeader header;
  header.sessionToken = recipient.sessionToken;
  header.sequence = recipient.nextSequence;
  ++recipient.nextSequence;
  send(network::OutgoingDatagram{
      .destination = recipient.endpoint,
      .payload = network::DatagramCodec::encode(header, spawned)});
}

}  // namespace rtype::server
