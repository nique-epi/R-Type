#include "LuauDeclarations.hpp"
#include <memory>
#include <utility>
#include "BehaviourRegistry.hpp"
#include "LuauBehaviour.hpp"
#include "LuauBehaviourParts.hpp"
#include "LuauHitHandler.hpp"
#include "LuauHitHandlerParts.hpp"

namespace rtype::game {

void LuauDeclarations::behaviour(LuauBehaviourParts parts) {
  behaviours_.push_back(std::make_unique<LuauBehaviour>(std::move(parts)));
}

void LuauDeclarations::hitHandler(LuauHitHandlerParts parts) {
  hitHandlers_.push_back(std::make_unique<LuauHitHandler>(std::move(parts)));
}

void LuauDeclarations::handOverTo(BehaviourRegistry& registry) {
  for (std::unique_ptr<LuauBehaviour>& behaviour : behaviours_) {
    registry.add(std::move(behaviour));
  }
  behaviours_.clear();
  for (std::unique_ptr<LuauHitHandler>& handler : hitHandlers_) {
    registry.addHitHandler(std::move(handler));
  }
  hitHandlers_.clear();
}

}  // namespace rtype::game
