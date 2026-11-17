#include "SummoningSystem.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "Summoning.hpp"
#include "TimeConstants.hpp"

namespace rtype::game {

void SummoningSystem::update(engine::ComponentRegistry& components,
                             [[maybe_unused]] engine::Duration elapsed) {
  awoken_.clear();
  components.forEach<Summoning, Summoning>(
      [this](engine::Entity body, Summoning& summoning, Summoning&) {
        if (--summoning.ticksLeft == 0) {
          awoken_.push_back(body);
        }
      });
  for (const engine::Entity body : awoken_) {
    components.remove<Summoning>(body);
  }
}

}  // namespace rtype::game
