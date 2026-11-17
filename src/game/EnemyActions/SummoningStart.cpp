#include "SummoningStart.hpp"
#include <cstddef>
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "Gun.hpp"
#include "Summoning.hpp"

namespace rtype::game {

void startSummoning(engine::ComponentRegistry& components, engine::Entity body,
                    std::size_t ticks) {
  components.add(body, Summoning{.ticksLeft = ticks});
  if (auto* const gun = components.get<Gun>(body)) {
    gun->ticksUntilReady += ticks;
  }
}

}  // namespace rtype::game
