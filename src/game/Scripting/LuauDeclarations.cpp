#include "LuauDeclarations.hpp"
#include <memory>
#include <utility>
#include "BehaviourRegistry.hpp"
#include "LuauBehaviour.hpp"
#include "LuauBehaviourParts.hpp"

namespace rtype::game {

void LuauDeclarations::behaviour(LuauBehaviourParts parts) {
  behaviours_.push_back(std::make_unique<LuauBehaviour>(std::move(parts)));
}

void LuauDeclarations::handOverTo(BehaviourRegistry& registry) {
  for (std::unique_ptr<LuauBehaviour>& behaviour : behaviours_) {
    registry.add(std::move(behaviour));
  }
  behaviours_.clear();
}

}  // namespace rtype::game
