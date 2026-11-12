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

namespace {

/** @returns The item of @p items whose name() is @p name, or nullptr when none
 * is: a behaviour or a hit handler. */
template <typename Named>
Named* namedIn(const std::vector<std::unique_ptr<Named>>& items,
               std::string_view name) {
  const auto found =
      std::ranges::find_if(items, [name](const std::unique_ptr<Named>& item) {
        return item->name() == name;
      });
  return found == items.end() ? nullptr : found->get();
}

/** @returns The names of @p items, in their order. */
template <typename Named>
std::vector<std::string_view> namesOf(
    const std::vector<std::unique_ptr<Named>>& items) {
  std::vector<std::string_view> names;
  names.reserve(items.size());
  for (const std::unique_ptr<Named>& item : items) {
    names.push_back(item->name());
  }
  return names;
}

}  // namespace

void BehaviourRegistry::add(std::unique_ptr<IBehaviour> behaviour) {
  if (find(behaviour->name()) != nullptr) {
    throw DuplicateBehaviourException(behaviour->name());
  }
  behaviours_.push_back(std::move(behaviour));
}

const IBehaviour* BehaviourRegistry::find(std::string_view name) const {
  return namedIn(behaviours_, name);
}

std::vector<std::string_view> BehaviourRegistry::names() const {
  return namesOf(behaviours_);
}

void BehaviourRegistry::addHitHandler(std::unique_ptr<IHitHandler> handler) {
  if (findHitHandler(handler->name()) != nullptr) {
    throw DuplicateHitHandlerException(handler->name());
  }
  hitHandlers_.push_back(std::move(handler));
}

IHitHandler* BehaviourRegistry::findHitHandler(std::string_view name) const {
  return namedIn(hitHandlers_, name);
}

std::vector<std::string_view> BehaviourRegistry::hitHandlerNames() const {
  return namesOf(hitHandlers_);
}

}  // namespace rtype::game
