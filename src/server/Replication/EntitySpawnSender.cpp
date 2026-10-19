#include "EntitySpawnSender.hpp"
#include "ConnectedPlayer.hpp"
#include "EntitySpawned.hpp"
#include "SessionDatagram.hpp"

namespace rtype::server {

void sendEntitySpawned(const SendDatagram& send, ConnectedPlayer& recipient,
                       const network::EntitySpawned& spawned) {
  send(sessionDatagram(recipient, spawned));
}

}  // namespace rtype::server
