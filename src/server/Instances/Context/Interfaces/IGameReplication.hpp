#pragma once

#include <cstdint>
#include "ComponentRegistry.hpp"
#include "ConnectedPlayer.hpp"
#include "World.hpp"

namespace rtype::server {

/**
 * @brief What the rules of a game ask of the part that tells the players about
 * it: the module that replicates the game provides it as a service, and the
 * rules call it at the points of the tick where the players must be told.
 *
 * Every call is made at its place in the tick and sends what it has to before
 * it returns, so the order of the datagrams to a player is the order of the
 * calls.
 */
class IGameReplication {
 public:
  virtual ~IGameReplication() = default;

  /**
   * @brief Tells the replication the game has a new world: everything that
   * works on one world is built again for @p world, and the end of the
   * previous game is forgotten.
   */
  virtual void onWorldBuilt(game::World& world) = 0;

  /**
   * @brief Gives @p player a ship in the world, unless it has one, and
   * announces it to the players.
   * @throws NetworkIdExhaustedException when every network id was given.
   */
  virtual void onPlayerJoined(ConnectedPlayer& player) = 0;

  /**
   * @brief Tells the players the room, on a tick of the wait for players.
   */
  virtual void onWaitingTick() = 0;

  /**
   * @brief Notes that the game ended by itself: the players are told the
   * result from the next onFinishedTick().
   */
  virtual void onGameFinished() = 0;

  /**
   * @brief Tells the players the result of the game, on a tick after its end.
   * Does nothing when the game was ended without a result.
   */
  virtual void onFinishedTick() = 0;

  /**
   * @brief Sends the players the world, on a tick of the game in progress.
   */
  virtual void onProgressTick(std::uint32_t tick,
                              engine::ComponentRegistry& components) = 0;
};

}  // namespace rtype::server
