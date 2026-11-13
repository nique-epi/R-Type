#pragma once

#include <cstdint>
#include <memory>
#include "BotSight.hpp"
#include "EntityMessage.hpp"
#include "IBotPilot.hpp"
#include "IPlayerInputLink.hpp"
#include "IWaitingRoom.hpp"
#include "PlayerInputPacer.hpp"
#include "TimeConstants.hpp"

namespace rtype::tools {

/**
 * @brief The hands of a bot: once its game started, it sends a PlayerInput of
 * what its pilot holds every PLAYER_INPUT_INTERVAL: a bot predicts nothing,
 * where the game screen sends one input per simulation tick. Before the game
 * starts, and once it ended, neither the pilot nor the pace move, and nothing
 * is sent. Each input repeats the ones the server has not confirmed, so the
 * sender confirms them from the world states, as the game screen does (see
 * confirm()).
 *
 * In a bot, the link is the ServerSession of its connection, whose first input
 * of a game also sends the GameLoaded the server waits for (see
 * ServerSession::sendPlayerInput()).
 */
class BotInputSender {
 public:
  /**
   * @param pilot What decides what the bot holds.
   * @param room Where the sender reads whether the game started; it must
   * outlive the sender.
   * @param link Where the inputs go; it must outlive the sender.
   */
  BotInputSender(std::unique_ptr<IBotPilot> pilot,
                 const client::IWaitingRoom& room,
                 client::IPlayerInputLink& link);

  /**
   * @brief Moves the pilot, which knows @p sight, and the pace by a frame of
   * @p elapsed while the game runs, and sends what the pilot holds when an
   * input is due.
   * @returns Whether an input went out in this frame.
   */
  [[nodiscard]] bool advance(engine::Duration elapsed, const BotSight& sight);

  /**
   * @brief Confirms the inputs up to the newest one the server applied to the
   * ship of the player @p playerNumber, when @p message is a world state that
   * lists that ship (see client::ownShipOf()): the next inputs no longer
   * repeat them. Any other message confirms nothing.
   */
  void confirm(const client::EntityMessage& message, std::uint8_t playerNumber);

 private:
  const client::IWaitingRoom* room_;
  client::IPlayerInputLink* link_;
  std::unique_ptr<IBotPilot> pilot_;
  client::PlayerInputPacer pacer_;
};

}  // namespace rtype::tools
