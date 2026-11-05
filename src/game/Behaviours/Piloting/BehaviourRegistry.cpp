#include "BehaviourRegistry.hpp"
#include <algorithm>
#include <memory>
#include <string_view>
#include <utility>
#include "GameException.hpp"
#include "IBehaviour.hpp"

namespace rtype::game {

void BehaviourRegistry::add(std::unique_ptr<IBehaviour> behaviour) {
  if (find(behaviour->name()) != nullptr) {
    throw DuplicateBehaviourException(behaviour->name());
  }
  behaviours_.push_back(std::move(behaviour));
}

const IBehaviour& BehaviourRegistry::named(std::string_view name) const {
  const IBehaviour* const behaviour = find(name);
  if (behaviour == nullptr) {
    throw UnknownBehaviourException(name);
  }
  return *behaviour;
}

const IBehaviour* BehaviourRegistry::find(std::string_view name) const {
  const auto found = std::ranges::find_if(
      behaviours_, [name](const std::unique_ptr<IBehaviour>& behaviour) {
        return behaviour->name() == name;
      });
  return found == behaviours_.end() ? nullptr : found->get();
}

}  // namespace rtype::game
