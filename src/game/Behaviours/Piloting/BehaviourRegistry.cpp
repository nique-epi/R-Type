#include "BehaviourRegistry.hpp"
#include <algorithm>
#include <memory>
#include <string_view>
#include <utility>
#include <vector>
#include "GameException.hpp"
#include "IBehaviour.hpp"
#include "IHitHandler.hpp"

namespace rtype::game {

void BehaviourRegistry::add(std::unique_ptr<IBehaviour> behaviour) {
  if (find(behaviour->name()) != nullptr) {
    throw DuplicateBehaviourException(behaviour->name());
  }
  behaviours_.push_back(std::move(behaviour));
}

const IBehaviour* BehaviourRegistry::find(std::string_view name) const {
  const auto found = std::ranges::find_if(
      behaviours_, [name](const std::unique_ptr<IBehaviour>& behaviour) {
        return behaviour->name() == name;
      });
  return found == behaviours_.end() ? nullptr : found->get();
}

std::vector<std::string_view> BehaviourRegistry::names() const {
  std::vector<std::string_view> names;
  names.reserve(behaviours_.size());
  for (const std::unique_ptr<IBehaviour>& behaviour : behaviours_) {
    names.push_back(behaviour->name());
  }
  return names;
}

void BehaviourRegistry::addHitHandler(std::unique_ptr<IHitHandler> handler) {
  if (findHitHandler(handler->name()) != nullptr) {
    throw DuplicateHitHandlerException(handler->name());
  }
  hitHandlers_.push_back(std::move(handler));
}

IHitHandler* BehaviourRegistry::findHitHandler(std::string_view name) const {
  const auto found = std::ranges::find_if(
      hitHandlers_, [name](const std::unique_ptr<IHitHandler>& handler) {
        return handler->name() == name;
      });
  return found == hitHandlers_.end() ? nullptr : found->get();
}

std::vector<std::string_view> BehaviourRegistry::hitHandlerNames() const {
  std::vector<std::string_view> names;
  names.reserve(hitHandlers_.size());
  for (const std::unique_ptr<IHitHandler>& handler : hitHandlers_) {
    names.push_back(handler->name());
  }
  return names;
}

}  // namespace rtype::game
