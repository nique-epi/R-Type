#pragma once

#include <memory>
#include <string>
#include "ConnectedPlayer.hpp"
#include "GameFlow.hpp"
#include "IGameModule.hpp"
#include "IGameReplication.hpp"
#include "InstallContext.hpp"
#include "ScoreKeeper.hpp"
#include "World.hpp"
#include "WorldSlot.hpp"

namespace rtype::server {

struct GameContext;

/**
 * @brief The module of a game that carries out the rules of R-Type: its
 * systems, its shots, its damage, its lives, its score and its waves, and the
 * way the game unfolds (see GameFlow). It owns the world of the game and
 * provides the flow to the game as IGameFlow.
 *
 * It requires the IGameReplication of the module before it, which tells the
 * players what the rules do. install() builds the GameFlow, which opens
 * nothing yet, provides it and adds the work of the tick; start() adds the
 * systems, subscribes the ScoreKeeper and has the flow build the first world;
 * stop() destroys the flow, then the keeper, which give their subscriptions
 * back to the bus. Until it begins, the flow is a game that waits for players
 * at tick 0, so the service answers from the end of install() to the start of
 * stop().
 *
 * start() also adds three functions to the hooks of the game: a player who
 * joins gets a ship through the replication; the ship of a player whose
 * session closes leaves the world and is announced on the event bus as an
 * EntityDestruction (Removed); each PlayerInput newer than the one kept goes
 * through noteTaps before it replaces it, so a direction pressed twice starts
 * a roll or a dash of the ship. A ScoreKeeper adds the points of each
 * EnemyKilled published on the event bus to the score of the player it names;
 * every score goes back to 0 with each new world.
 */
class RtypeRulesModule final : public IGameModule {
 public:
  [[nodiscard]] std::string name() const override;

  void install(GameContext& game, engine::InstallContext& install) override;

  void start() override;

  void stop() override;

  /**
   * @returns The world of the game. A new one replaces it when the game waits
   * for players again, so do not keep the reference across ticks.
   */
  [[nodiscard]] game::World& world();

 private:
  /** @brief Adds the functions the rules run when a player joins, leaves or
   * sends an input to the hooks of the game. */
  void addSessionHooks();

  /** @brief Removes the ship of @p departed from the world, when it has one,
   * and announces it on the event bus as an EntityDestruction (Removed). */
  void removeShipOf(const ConnectedPlayer& departed);

  GameContext* game_{nullptr};
  IGameReplication* replication_{nullptr};
  WorldSlot world_;
  std::unique_ptr<GameFlow> flow_;
  std::unique_ptr<ScoreKeeper> scoreKeeper_;
};

}  // namespace rtype::server
