#pragma once

#include <vector>
#include "Entity.hpp"
#include "ISystem.hpp"
#include "TimeConstants.hpp"

namespace rtype::engine {

class ComponentRegistry;

}  // namespace rtype::engine

namespace rtype::client {

/**
 * @brief Counts the HitFlash of every entity down, and removes the ones whose
 * time is spent, so the entity is drawn in its own colors again.
 *
 * A flash whose time reaches zero during a step is removed at the end of that
 * step: the picture drawn after the step no longer shows it.
 */
class HitFlashSystem final : public engine::ISystem {
 public:
  void update(engine::ComponentRegistry& components,
              engine::Duration elapsed) override;

 private:
  std::vector<engine::Entity> spentFlashes_;
};

}  // namespace rtype::client
