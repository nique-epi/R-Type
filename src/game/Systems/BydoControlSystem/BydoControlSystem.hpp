#pragma once

#include "ISystem.hpp"
#include "TimeConstants.hpp"

namespace rtype::engine {

class ComponentRegistry;

}  // namespace rtype::engine

namespace rtype::game {

/**
 * @brief The artificial intelligence of the Bydos: writes the Intent of every
 * entity of type Enemy.
 *
 * A Bydo wants to shoot as long as it is alive; how often it fires is the
 * interval of its Gun, and how it moves is its Velocity and its
 * MovementPattern (see PatternSystem). MovementSystem and the weapon system act
 * on the Intent and never ask which controller wrote it, so a player controller
 * and this one are interchangeable. Entities that are not Enemy keep their
 * Intent. Run it before the weapon system.
 */
class BydoControlSystem final : public engine::ISystem {
 public:
  void update(engine::ComponentRegistry& components,
              engine::Duration elapsed) override;
};

}  // namespace rtype::game
