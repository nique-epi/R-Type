#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include "AppliedInput.hpp"
#include "ArrivalLedger.hpp"
#include "Endpoint.hpp"
#include "EntityIdConstants.hpp"
#include "LivesConstants.hpp"
#include "RoundTripMeter.hpp"
#include "SentStates.hpp"
#include "TimeConstants.hpp"
#include "UnacknowledgedMessages.hpp"

namespace rtype::server {

/**
 * @brief A player connected to the game: who the player is to the others, and
 * the session the server keeps with them.
 */
struct ConnectedPlayer {
  /** @brief FIRST_PLAYER_NUMBER to MAXIMUM_PLAYERS; it also picks the color
   * of the player. */
  std::uint8_t playerNumber{};
  /** @brief 1 to MAXIMUM_NICKNAME_SIZE bytes of UTF-8, as the request carried
   * it. */
  std::string nickname;
  /** @brief The address and port the player sends from, where every datagram
   * to the player goes. */
  network::Endpoint endpoint;
  /** @brief Drawn at random when the player was accepted, never 0; every
   * datagram of the session carries it. */
  std::uint64_t sessionToken{};
  /** @brief Sequence of the next datagram sent to the player: 0 for the first
   * one, then one more each time, wrapping from 65535 to 0. */
  std::uint16_t nextSequence{};
  /** @brief The network id of the ship of the player, NO_ENTITY_ID while it
   * has none. */
  std::uint32_t shipEntityId{identifiers::NO_ENTITY_ID};
  /** @brief The points the player earned in the current game; back to 0 when a
   * new world is built. */
  std::uint32_t score{0};
  /** @brief The lives the player has while it has no ship, the next ship
   * included: game::STARTING_LIVES when it joins and when a new world is
   * built, one less each time its ship is destroyed. At 0 the player has no
   * ship and watches. While the player has a ship, the Lives component of the
   * ship holds the lives instead, and this field keeps the value the ship was
   * given. */
  std::uint8_t livesWithoutShip{game::STARTING_LIVES};
  /** @brief Ticks of the game left before the ship of the player comes back
   * after it was destroyed: RESPAWN_DELAY_TICKS then, one less at each tick.
   * 0 while the player has a ship, or no life left. LifeKeeper counts it. */
  std::size_t ticksUntilRespawn{0};
  /** @brief Whether the player declared itself ready to start; cleared when the
   * game waits for players again. */
  bool ready{false};
  /** @brief Whether the player told the game it shows the game screen, since
   * the game last started waiting for it; cleared when the game does. */
  bool loaded{false};
  /** @brief The datagrams accepted from the player, which every datagram to it
   * acknowledges, and how many of the datagrams it sent never arrived.
   * DatagramReader keeps it. */
  ArrivalLedger arrivals;
  /** @brief The highest inputNumber the player sent during the session, the
   * key that tells a newer input from a late one; empty until the first input
   * arrives. The input itself is held by the Intent of the ship of the player.
   * InputKeeper keeps it. */
  std::optional<std::uint32_t> newestInputNumber;
  /** @brief The newest input applied to the ship of the player; empty until
   * one is applied and cleared with newestInputNumber. An input is applied when
   * the hooks of the game see it, so this number never runs ahead of the ship,
   * as newestInputNumber does when inputs are held before they are applied. It
   * also tells an input that follows earlier ones from the first of the
   * session, which newestInputNumber cannot, since it runs ahead by the input
   * delay. DelayedInputs keeps it. */
  std::optional<AppliedInput> appliedInput;
  /** @brief Ticks the game ran since it last read a datagram of the player;
   * the player is away once it reaches SESSION_TIMEOUT_TICKS, and the session
   * closes once it reaches SESSION_CLOSING_TICKS.
   */
  std::size_t ticksSinceReceived{0};
  /** @brief Whether the server took the player for gone: its seat, ship, score
   * and lives are kept, its input number is cleared and it does not count for
   * the ready check. The next datagram read from the player clears it. */
  bool away{false};
  /** @brief The real time since the server last sent the player a Heartbeat,
   * added at the start of each tick, a stall of the server included, since
   * the client waits in real time: the next Heartbeat goes out once it reaches
   * TIME_BETWEEN_HEARTBEATS, whatever else the server sent, and starts it over
   * (see sendHeartbeats()). */
  engine::Duration sinceHeartbeat{};
  /** @brief The round trip between the server and the client of the player,
   * measured from their Heartbeats, and its jitter. Heartbeats.cpp keeps it.
   */
  network::RoundTripMeter roundTrip;
  /** @brief The guaranteed messages sent to the player that it has not
   * acknowledged yet. GameSessions drops those the datagrams of the player
   * acknowledge, and makes them older at the start of each tick;
   * guaranteeToEveryPlayer() and resendGuaranteedMessages() send them. */
  network::UnacknowledgedMessages unacknowledged;
  /** @brief The world states sent to the player and the newest one it
   * acknowledged in full. GameSessions gives it the acknowledgement of each
   * datagram of the player; the world state sender writes each state against
   * the acknowledged one. */
  SentStates sentStates;
  /** @brief When the acceptor last sent this player its ConnectionAccepted,
   * on the clock of its cookies; empty before the first one. It paces the
   * acceptance repeated to a player who asks again (see
   * CONNECTION_ACCEPTANCE_REPEAT_INTERVAL), and leaves with the player. */
  std::optional<engine::Duration> lastAcceptedAt;
};

}  // namespace rtype::server
