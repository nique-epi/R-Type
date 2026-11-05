#pragma once

#include <memory>
#include <string_view>
#include <vector>
#include "IBehaviour.hpp"

namespace rtype::game {

/**
 * @brief The behaviours a body may be given, found by their name.
 *
 * It owns them, so a Pilot may keep a reference to one for as long as the
 * registry lives. A behaviour is looked up when a body appears, never during a
 * tick.
 */
class BehaviourRegistry {
 public:
  /**
   * @brief Adds @p behaviour under its name().
   * @throws DuplicateBehaviourException When a behaviour of that name is
   * already in the registry.
   */
  void add(std::unique_ptr<IBehaviour> behaviour);

  /**
   * @returns The behaviour named @p name.
   * @throws UnknownBehaviourException When none is.
   */
  [[nodiscard]] const IBehaviour& named(std::string_view name) const;

 private:
  /** @returns The behaviour named @p name, or nullptr when none is. */
  [[nodiscard]] const IBehaviour* find(std::string_view name) const;

  std::vector<std::unique_ptr<IBehaviour>> behaviours_;
};

}  // namespace rtype::game
