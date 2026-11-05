#include "RtypeBehaviours.hpp"
#include <memory>
#include "BehaviourRegistry.hpp"
#include "StraightBehaviour.hpp"
#include "WeaveBehaviour.hpp"

namespace rtype::game {

BehaviourRegistry rtypeBehaviours() {
  BehaviourRegistry behaviours;
  behaviours.add(std::make_unique<StraightBehaviour>());
  behaviours.add(std::make_unique<WeaveBehaviour>());
  return behaviours;
}

}  // namespace rtype::game
