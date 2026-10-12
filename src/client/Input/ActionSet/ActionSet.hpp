#pragma once

#include <cstdint>
#include "PlayerAction.hpp"

namespace rtype::client {

/**
 * @brief The actions held at a given moment, one bit per action.
 */
class ActionSet {
 public:
  void add(PlayerAction action);
  void remove(PlayerAction action);
  void clear();

  [[nodiscard]] bool has(PlayerAction action) const;

  /** @returns The `actions` byte of a `PlayerInput`: 0 when nothing is held. */
  [[nodiscard]] std::uint8_t bits() const;

 private:
  [[nodiscard]] static std::uint8_t maskOf(PlayerAction action);

  std::uint8_t bits_{0};
};

}  // namespace rtype::client
