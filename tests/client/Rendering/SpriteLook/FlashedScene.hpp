#pragma once

#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"
#include "HitFlashSystem.hpp"
#include "TimeConstants.hpp"

/** @brief Registries of flashed entities, and the system that ends their
 * flashes. */
class FlashedScene {
 public:
  FlashedScene();

  /** @returns A drawn entity, flashed white for @p remaining. */
  rtype::engine::Entity flashed(rtype::engine::Duration remaining);

  void play(rtype::engine::Duration elapsed);

  [[nodiscard]] bool isFlashed(rtype::engine::Entity entity) const;

  [[nodiscard]] bool isAlive(rtype::engine::Entity entity) const;

 private:
  rtype::engine::EntityRegistry entities_;
  rtype::engine::ComponentRegistry components_;
  rtype::client::HitFlashSystem system_;
};
