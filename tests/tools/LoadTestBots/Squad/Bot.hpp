#pragma once

#include <memory>
#include <string_view>
#include <vector>
#include "BossTally.hpp"
#include "BotInputSender.hpp"
#include "BotReport.hpp"
#include "BotSight.hpp"
#include "BotStanding.hpp"
#include "BotTally.hpp"
#include "Endpoint.hpp"
#include "EntityMessage.hpp"
#include "IBotPilot.hpp"
#include "PlayerConnection.hpp"
#include "TimeConstants.hpp"
#include "WaitingRoom.hpp"

namespace rtype::tools {

/**
 * @brief A player with no window: the PlayerConnection of the client, driven
 * by a BotInputSender instead of a keyboard.
 *
 * At each update it reads what the server sent and sends what is due, drains
 * the entity messages of the game, as the game screen does, keeps what they
 * show in its BotSight, and has its BotTally count them and the traffic of its
 * socket, decoded and as the network carried it, and its BotInputSender confirm
 * the inputs the world states acknowledge; then its sender plays when the game
 * started, and the tally counts each input it sent. It asks to be ready only
 * when its squad tells it to (see asksReady()).
 */
class Bot {
 public:
  /** @param pilot What decides what it holds. */
  explicit Bot(std::unique_ptr<IBotPilot> pilot);

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

  /** @returns What it knows of its game. */
  [[nodiscard]] const BotSight& sight() const;

  /** @brief Leaves the session (see PlayerConnection::leave()), then closes
   * the socket. */
  void leave();

 private:
  /** @brief Drains the entity messages, shows them to the sight, has the
   * tally count them, the time
   * of the frame of @p elapsed and the traffic of the socket, and has the
   * sender confirm the inputs each world state acknowledges. The boss tally
   * reads the lives lost after each message, before that message can end the
   * fight: a life lost in the frame that ends a fight still counts in it. */
  void readReceived(engine::Duration elapsed);

  client::PlayerConnection connection_;
  const client::WaitingRoom* room_;
  BotSight sight_;
  BotInputSender inputs_;
  std::vector<client::EntityMessage> received_;
  BotTally tally_;
  BossTally boss_;
};

}  // namespace rtype::tools
