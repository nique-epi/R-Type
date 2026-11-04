#pragma once

#include <cstdint>
#include "BotPilot.hpp"
#include "IPlayerInputLink.hpp"
#include "IWaitingRoom.hpp"
#include "PlayerInputPacer.hpp"
#include "TimeConstants.hpp"

namespace rtype::tools {

/**
 * @brief The hands of a bot: once its game started, it sends a PlayerInput of
 * what its BotPilot holds every PLAYER_INPUT_INTERVAL, as the game screen does
 * without client prediction. Before the game starts, and once it ended,
 * neither the pilot nor the pace move, and nothing is sent.
 *
 * In a bot, the link is the ServerSession of its connection, whose first input
 * of a game also sends the GameLoaded the server waits for (see
 * ServerSession::sendPlayerInput()).
 */
class BotInputSender {
 public:
  /**
   * @param seed The seed the pilot draws from.
   * @param room Where the sender reads whether the game started; it must
   * outlive the sender.
   * @param link Where the inputs go; it must outlive the sender.
   */
  BotInputSender(std::uint32_t seed, const client::IWaitingRoom& room,
                 client::IPlayerInputLink& link);

  /**
   * @brief Moves the pilot and the pace by a frame of @p elapsed while the
   * game runs, and sends what the pilot holds when an input is due.
   * @returns Whether an input went out in this frame.
   */
  [[nodiscard]] bool advance(engine::Duration elapsed);

 private:
  const client::IWaitingRoom* room_;
  client::IPlayerInputLink* link_;
  BotPilot pilot_;
  client::PlayerInputPacer pacer_;
};

}  // namespace rtype::tools
