#pragma once

#include <cstdint>
#include <string>
#include "Endpoint.hpp"

namespace rtype::server {

/**
 * @brief A player connected to the game: who the player is to the others, and
 * the session the server keeps with them.
 */
struct ConnectedPlayer {
  /** @brief FIRST_PLAYER_NUMBER to MAXIMUM_PLAYERS; it also picks the color
   * of the player. */
  std::uint8_t playerNumber;
  /** @brief 1 to MAXIMUM_NICKNAME_SIZE bytes of UTF-8, as the request carried
   * it. */
  std::string nickname;
  /** @brief The address and port the player sends from, where every datagram
   * to the player goes. */
  network::Endpoint endpoint;
  /** @brief Drawn at random when the player was accepted, never 0; every
   * datagram of the session carries it. */
  std::uint64_t sessionToken;
  /** @brief Sequence of the next datagram sent to the player: 0 for the first
   * one, then one more each time, wrapping from 65535 to 0. */
  std::uint16_t nextSequence;
};

}  // namespace rtype::server
