#pragma once

#include <cstddef>
#include <memory>
#include <span>
#include <vector>
#include "Endpoint.hpp"
#include "EntitySheets.hpp"
#include "EventBus.hpp"
#include "GameInstance.hpp"
#include "IGameBuses.hpp"

namespace rtype::server {

/**
 * @brief The games the server runs, in the order they were created, each with
 * the bus it publishes on, and the rule that decides which one a connection
 * request goes to.
 *
 * Each game has a bus of its own: the rules and the broadcaster of a game
 * subscribe to it, so an event of one game never reaches the rules or the
 * players of another. The games module owns it and provides it as a service,
 * so the lobby asks here where a player goes instead of knowing the rule
 * itself. It starts empty: the games module adds the games when it starts,
 * after it keeps the entity sheets they are made from, which the lobby reads
 * to tell each player the entity types of the server. It is also the
 * IGameBuses of the server: a module that follows every game reads their buses
 * through it.
 */
class Games : public IGameBuses {
 public:
  /** @brief Adds @p game after the ones already there, with @p events, the bus
   * it publishes on, which is kept alive as long as the game. */
  void add(std::unique_ptr<engine::EventBus> events,
           std::unique_ptr<GameInstance> game);

  /**
   * @returns The game every new player joins: the first one that waits for
   * players and has a free seat (see firstJoinableGame()), or the first game
   * when none does, which then refuses the player.
   * @throws NoJoinableGameException when no game was added yet.
   */
  [[nodiscard]] GameInstance& joinable() const;

  /**
   * @returns The index in all() of the game a connection request from
   * @p sender goes to: the game whose roster holds the sender, or else the
   * joinable() one.
   * @throws NoJoinableGameException when no game was added yet.
   */
  [[nodiscard]] std::size_t placeOf(const network::Endpoint& sender) const;

  /** @brief Delivers the events each game published since the last call, one
   * bus after the other, in the order of the games. */
  void dispatchEvents();

  /** @returns Every game, in the order they were added. */
  [[nodiscard]] std::span<const std::unique_ptr<GameInstance>> all() const;

  /** @returns The bus of each game, in the order of all(). */
  [[nodiscard]] std::span<const std::unique_ptr<engine::EventBus>> buses()
      const override;

  /** @brief Keeps @p types, the entity sheets every game of the list makes
   * its entities from (GameSettings::types); they must outlive the list. */
  void keepEntityTypes(const game::EntitySheets& types);

  /** @returns The entity sheets every game makes its entities from, nullptr
   * before keepEntityTypes(): the games module keeps them when it starts,
   * before it adds the games. */
  [[nodiscard]] const game::EntitySheets* entityTypes() const;

 private:
  [[nodiscard]] std::size_t joinableIndex() const;

  // The buses are declared before the games, so each game is destroyed before
  // the bus it holds a reference to.
  std::vector<std::unique_ptr<engine::EventBus>> events_;
  std::vector<std::unique_ptr<GameInstance>> games_;
  const game::EntitySheets* entityTypes_{nullptr};
};

}  // namespace rtype::server
