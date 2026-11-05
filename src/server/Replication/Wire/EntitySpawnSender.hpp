#pragma once

#include <cstdint>
#include <functional>
#include "ConnectedPlayer.hpp"
#include "EntitySpawned.hpp"
#include "EntityTypeNumber.hpp"
#include "OutgoingDatagram.hpp"
#include "Position.hpp"

namespace rtype::server {

/**
 * @brief What is called for each datagram to send.
 */
using SendDatagram = std::function<void(network::OutgoingDatagram)>;

/**
 * @returns The EntitySpawned that announces the entity @p networkId, of the
 * type @p type, at @p position: what the waves, the weapons and the ships
 * announce.
 */
[[nodiscard]] network::EntitySpawned entitySpawnedOf(
    std::uint32_t networkId, game::EntityTypeNumber type,
    game::Position position);

/**
 * @brief Sends @p recipient an EntitySpawned in its own datagram, carrying the
 * session token of the recipient and the next sequence of its session, which
 * advances by one.
 * @param send Must not be empty.
 */
void sendEntitySpawned(const SendDatagram& send, ConnectedPlayer& recipient,
                       const network::EntitySpawned& spawned);

}  // namespace rtype::server
