#include "WorldSlot.hpp"
#include <cstddef>
#include <memory>
#include "World.hpp"

namespace rtype::server {

WorldSlot::WorldSlot() : world_(std::make_unique<game::World>()) {}

game::World& WorldSlot::current() { return *world_; }

game::World& WorldSlot::renew() {
  world_ = std::make_unique<game::World>();
  return *world_;
}

std::size_t WorldSlot::entityCount() const { return world_->entityCount(); }

}  // namespace rtype::server
