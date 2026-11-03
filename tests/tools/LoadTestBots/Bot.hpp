#pragma once

#include <cstdint>
#include <optional>
#include <string_view>
#include <vector>
#include "BotPilot.hpp"
#include "BotReport.hpp"
#include "BotStanding.hpp"
#include "Endpoint.hpp"
#include "EntityMessage.hpp"
#include "PlayerConnection.hpp"
#include "PlayerInputPacer.hpp"
#include "TimeConstants.hpp"

namespace rtype::tools {

/**
 * @brief A player with no window: the PlayerConnection of the client, driven
 * by a BotPilot instead of a keyboard.
 *
 * Once its game started, it sends a PlayerInput of what its pilot holds every
 * PLAYER_INPUT_INTERVAL, as the game screen does without client prediction;
 * the first input of a game also sends the GameLoaded the server waits for. It
 * drains the entity messages of the game at each update, as the game screen
 * does, and counts the world states and the ends of game among them. It asks
 * to be ready only when its squad tells it to (see asksReady()).
 */
class Bot {
 public:
  /** @param seed The seed its pilot draws from. */
  explicit Bot(std::uint32_t seed);

  /**
   * @brief Opens its socket, then starts joining @p server as @p nickname (see
   * ServerSession::connect()).
   * @throws SocketOpenException when the system refuses to open its socket.
   */
  void join(const network::Endpoint& server, std::string_view nickname);

  /**
   * @brief Moves the bot by @p elapsed: reads what the server sent and sends
   * what is due, counts what it received, then plays when its game started.
   */
  void update(engine::Duration elapsed);

  /** @brief Asks the server to set the bot ready (see
   * WaitingRoom::setReady()). */
  void askReady();

  [[nodiscard]] BotStanding standing() const;
  [[nodiscard]] BotReport report() const;

  /** @brief Leaves the session (see PlayerConnection::leave()), then closes
   * the socket. */
  void leave();

 private:
  /**
   * @brief Drains the entity messages, counts them, and adds @p elapsed to
   * the time in game while a game it received a world state of runs: from that
   * state to the GameEnded, or to the end of the session.
   *
   * The frame that drains the first world state counts, the one that drains
   * the GameEnded does not. Each message arrived during the frame that drains
   * it, so the start of a game counts up to a frame too much and its end up to
   * a frame too little: they balance, where counting both frames would add a
   * frame to every game.
   */
  void countReceived(engine::Duration elapsed);

  /**
   * @brief Counts @p message when it is a world state of a new tick or the
   * end of the game.
   *
   * Comparing with the newest tick counted is enough: the server sends every
   * part of a tick before the next tick, and the session drops a datagram that
   * is not newer than the newest one it accepted, so a part of an older tick
   * never arrives after a newer one.
   */
  void count(const client::EntityMessage& message);

  /** @brief Sends what the pilot holds when an input is due, once the game
   * started. */
  void play(engine::Duration elapsed);

  client::PlayerConnection connection_;
  BotPilot pilot_;
  client::PlayerInputPacer pacer_;
  std::vector<client::EntityMessage> received_;
  BotReport counts_;
  /** @brief The tick of the newest world state of the game in progress;
   * nothing before its first one, and once it ended. */
  std::optional<std::uint32_t> newestTick_;
};

}  // namespace rtype::tools
