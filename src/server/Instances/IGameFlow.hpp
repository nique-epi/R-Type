#pragma once

#include <cstdint>
#include "ConnectedPlayer.hpp"
#include "GameState.hpp"

namespace rtype::server {

/**
 * @brief What a game shows of the way it unfolds, whatever the rules of the
 * game: the module that carries them out provides it as a service, and the
 * GameInstance answers through it.
 */
class IGameFlow {
 public:
  virtual ~IGameFlow() = default;

  [[nodiscard]] virtual GameState state() const = 0;

  /**
   * @returns How many ticks were simulated since the game started: 0 while it
   * waits for players, and again once it waits after a game.
   */
  [[nodiscard]] virtual std::uint32_t tick() const = 0;

  /**
   * @brief Gives @p player, who just took a place in the roster, a ship in the
   * world and announces it to the players.
   * @throws NetworkIdExhaustedException when every network id was given.
   */
  virtual void onPlayerJoined(ConnectedPlayer& player) = 0;

  /**
   * @brief Ends the game in progress. Does nothing in any other state.
   */
  virtual void finish() = 0;
};

}  // namespace rtype::server
