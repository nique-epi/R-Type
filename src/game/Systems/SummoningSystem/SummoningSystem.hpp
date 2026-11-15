#pragma once

#include <vector>
#include "Entity.hpp"
#include "ISystem.hpp"
#include "TimeConstants.hpp"

namespace rtype::engine {

class ComponentRegistry;

}  // namespace rtype::engine

namespace rtype::game {

/**
 * @brief Counts the Summoning of every body down, one tick at each tick, and
 * removes it once it reaches 0: the body takes part from the next tick.
 *
 * Run it after the other systems of the world: a body given a Summoning of N
 * ticks during a tick then stays out of N ticks of those systems, that tick
 * included when it was given before them.
 */
class SummoningSystem final : public engine::ISystem {
 public:
  void update(engine::ComponentRegistry& components,
              engine::Duration elapsed) override;

 private:
  std::vector<engine::Entity> awoken_;
};

}  // namespace rtype::game
