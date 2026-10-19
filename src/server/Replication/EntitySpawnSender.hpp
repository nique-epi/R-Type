#pragma once

#include <functional>
#include "ConnectedPlayer.hpp"
#include "EntitySpawned.hpp"
#include "OutgoingDatagram.hpp"

namespace rtype::server {

/**
 * @brief What is called for each datagram to send.
 */
using SendDatagram = std::function<void(network::OutgoingDatagram)>;

/**
 * @brief Sends @p recipient an EntitySpawned in its own datagram, carrying the
 * session token of the recipient and the next sequence of its session, which
 * advances by one.
 * @param send Must not be empty.
 */
void sendEntitySpawned(const SendDatagram& send, ConnectedPlayer& recipient,
                       const network::EntitySpawned& spawned);

}  // namespace rtype::server
