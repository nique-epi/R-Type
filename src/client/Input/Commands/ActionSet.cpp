#include "ActionSet.hpp"
#include <cstdint>
#include "PlayerAction.hpp"

namespace rtype::client {

void ActionSet::add(PlayerAction action) {
  bits_ = static_cast<std::uint8_t>(bits_ | maskOf(action));
}

void ActionSet::remove(PlayerAction action) {
  const auto held = static_cast<unsigned int>(bits_);
  const auto removed = static_cast<unsigned int>(maskOf(action));
  bits_ = static_cast<std::uint8_t>(held & ~removed);
}

void ActionSet::clear() { bits_ = 0; }

bool ActionSet::has(PlayerAction action) const {
  return (bits_ & maskOf(action)) != 0;
}

std::uint8_t ActionSet::bits() const { return bits_; }

std::uint8_t ActionSet::maskOf(PlayerAction action) {
  return static_cast<std::uint8_t>(1U << static_cast<unsigned int>(action));
}

}  // namespace rtype::client
