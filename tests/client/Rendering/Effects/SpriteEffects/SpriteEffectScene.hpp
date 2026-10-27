#pragma once

#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"
#include "Position.hpp"
#include "SpriteEffectSystem.hpp"
#include "TimeConstants.hpp"

/**
 * @brief An entity registry, its components and the system of the sprite
 * effects, with a way to add entities and the figure of a gain.
 */
class SpriteEffectScene {
 public:
  SpriteEffectScene();

  rtype::engine::Entity add(rtype::game::Position position);

  /** @returns The figure of a gain, GAIN_FIGURE_TEXT, just shown at START. */
  rtype::engine::Entity addFigure();

  /** @returns The figure of a gain, GAIN_FIGURE_TEXT, just shown at START,
   * steady as for a player who reduced flashes: it shows instead of
   * blinking. */
  rtype::engine::Entity addSteadyFigure();

  void run(rtype::engine::Duration elapsed);

  rtype::engine::ComponentRegistry& components();

  [[nodiscard]] bool isAlive(rtype::engine::Entity entity) const;

 private:
  rtype::engine::EntityRegistry entities_;
  rtype::engine::ComponentRegistry components_;
  rtype::client::SpriteEffectSystem system_;
};
