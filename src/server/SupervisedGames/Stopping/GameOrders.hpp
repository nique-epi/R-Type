#pragma once

#include <string>
#include "DatagramCodec.hpp"
#include "Endpoint.hpp"
#include "MessagePayload.hpp"
#include "NetworkConstants.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "SupervisedGame.hpp"

namespace rtype::server {

/**
 * @returns An order to @p game: @p message under its link token, to the port
 * it listens on, on the loopback. The game reads it in its supervisor link
 * module. Nothing repeats it, so the caller sends FAREWELL_COPIES of it.
 */
template <network::EncodableMessage Message>
[[nodiscard]] network::OutgoingDatagram orderTo(const SupervisedGame& game,
                                                const Message& message) {
  return network::OutgoingDatagram{
      .destination =
          network::Endpoint{
              .address = std::string{network::LOOPBACK_IPV4_ADDRESS},
              .port = game.port},
      .payload = network::DatagramCodec::encode(
          network::PacketHeader{.sessionToken = game.linkToken}, message)};
}

/** @returns The order that stops @p game, a Disconnection (see orderTo()):
 * the game says goodbye to its players and ends. */
[[nodiscard]] network::OutgoingDatagram stopOrderTo(const SupervisedGame& game);

}  // namespace rtype::server
