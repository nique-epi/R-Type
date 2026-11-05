#include "BuiltInBehaviours.hpp"
#include <memory>
#include "BehaviourRegistry.hpp"
#include "DrillbackBehaviour.hpp"
#include "ReaperBehaviour.hpp"
#include "ScatterBehaviour.hpp"
#include "StraightBehaviour.hpp"
#include "WeaveBehaviour.hpp"

namespace rtype::game {

BehaviourRegistry builtInBehaviours() {
  BehaviourRegistry behaviours;
  behaviours.add(std::make_unique<StraightBehaviour>());
  behaviours.add(std::make_unique<WeaveBehaviour>());
  behaviours.add(std::make_unique<ScatterBehaviour>());
  behaviours.add(std::make_unique<ReaperBehaviour>());
  behaviours.add(std::make_unique<DrillbackBehaviour>());
  return behaviours;
}

}  // namespace rtype::game
