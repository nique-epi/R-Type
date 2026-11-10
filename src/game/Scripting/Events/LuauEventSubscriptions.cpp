#include "LuauEventSubscriptions.hpp"
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include "LuauEventConstants.hpp"
#include "LuauEventSubscription.hpp"

namespace rtype::game {

bool LuauEventSubscriptions::full() const {
  return subscriptions_.size() >= MAXIMUM_SUBSCRIPTIONS_PER_ENTITY;
}

void LuauEventSubscriptions::add(
    std::unique_ptr<LuauEventSubscription> subscription) {
  subscriptions_.push_back(std::move(subscription));
}

void LuauEventSubscriptions::fail(const std::string& reason) {
  subscriptions_.clear();
  failure_ = reason;
}

const std::optional<std::string>& LuauEventSubscriptions::failure() const {
  return failure_;
}

void LuauEventSubscriptions::clear() {
  subscriptions_.clear();
  failure_.reset();
}

}  // namespace rtype::game
