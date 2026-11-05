#include "EntitySpawnSender.hpp"
#include <cstdint>
#include "ConnectedPlayer.hpp"
#include "EntitySpawned.hpp"
#include "EntityTypeNumber.hpp"
#include "Position.hpp"
#include "SessionDatagram.hpp"

namespace rtype::server {

network::EntitySpawned entitySpawnedOf(std::uint32_t networkId,
                                       game::EntityTypeNumber type,
                                       game::Position position) {
  return network::EntitySpawned{.entityId = networkId,
                                .entityType = type.number,
                                .x = position.x,
                                .y = position.y};
}

void sendEntitySpawned(const SendDatagram& send, ConnectedPlayer& recipient,
                       const network::EntitySpawned& spawned) {
  send(sessionDatagram(recipient, spawned));
}

}  // namespace rtype::server
