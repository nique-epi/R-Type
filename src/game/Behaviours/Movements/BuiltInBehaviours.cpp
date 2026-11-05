#include "BuiltInBehaviours.hpp"
#include <memory>
#include "BehaviourRegistry.hpp"
#include "ScatterBehaviour.hpp"
#include "StraightBehaviour.hpp"
#include "WeaveBehaviour.hpp"

namespace rtype::game {

BehaviourRegistry builtInBehaviours() {
  BehaviourRegistry behaviours;
  behaviours.add(std::make_unique<StraightBehaviour>());
  behaviours.add(std::make_unique<WeaveBehaviour>());
  behaviours.add(std::make_unique<ScatterBehaviour>());
  return behaviours;
}

}  // namespace rtype::game
