#pragma once

#include <memory>
#include "EntitySheets.hpp"
#include "EventBus.hpp"
#include "GameInstance.hpp"
#include "IGameEvents.hpp"

namespace rtype::server {

/**
 * @brief The game a game process runs, with the bus it publishes on.
 *
 * The games module owns it and provides it as a service, so the lobby, the
 * metrics and the supervisor link reach the game through it. The bus exists
 * from the start; the game only once the games module started, after it kept
 * the entity sheets the game is made from, which the lobby reads to tell each
 * player the entity types of the server. It is also the IGameEvents of the
 * process: a module that follows the game reads its bus through it.
 */
class RunningGame : public IGameEvents {
 public:
  /** @brief Keeps @p game, made to publish on events(); the games module calls
   * it once, when it starts. */
  void host(std::unique_ptr<GameInstance> game);

  /**
   * @returns The game.
   * @throws GameNotCreatedException before host().
   */
  [[nodiscard]] GameInstance& game() const;

  /** @returns The bus the game publishes on. */
  [[nodiscard]] engine::EventBus& events() override;

  /** @brief Keeps @p types, the entity sheets the game makes its entities from
   * (GameSettings::types); they must outlive the object. */
  void keepEntityTypes(const game::EntitySheets& types);

  /** @returns The entity sheets the game makes its entities from, nullptr
   * before keepEntityTypes(): the games module keeps them when it starts,
   * before it hosts the game. */
  [[nodiscard]] const game::EntitySheets* entityTypes() const;

 private:
  // The bus is declared before the game, so the game is destroyed before the
  // bus it holds a reference to.
  engine::EventBus events_;
  std::unique_ptr<GameInstance> game_;
  const game::EntitySheets* entityTypes_{nullptr};
};

}  // namespace rtype::server
