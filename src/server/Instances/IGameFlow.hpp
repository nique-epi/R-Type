#pragma once

#include <cstdint>
#include "ConnectedPlayer.hpp"
#include "GameState.hpp"
#include "PlayerInput.hpp"

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

  /**
   * @brief Called with the player whose session closes, after its seat is
   * freed and before its departure is announced: the game takes what it holds
   * of the player out of the world.
   */
  virtual void onPlayerLeaving(const ConnectedPlayer& player) = 0;

  /**
   * @brief Called with each PlayerInput newer than the one kept for
   * @p player, before it replaces that one, for the game to read the presses
   * the input holds.
   */
  virtual void onNewerInput(const ConnectedPlayer& player,
                            const network::PlayerInput& input) = 0;
};

}  // namespace rtype::server
