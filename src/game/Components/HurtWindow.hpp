#pragma once

#include <optional>
#include "TimeConstants.hpp"

namespace rtype::game {

/**
 * @brief When a box of a hurt zone hurts within the phase it names: from
 * `from` into the phase, until `until` into it (excluded), or to the end of
 * the phase without an end. A box that follows the frames of an attack has
 * one window per frame.
 */
struct HurtWindow {
  engine::Duration from{};
  std::optional<engine::Duration> until{std::nullopt};

  bool operator==(const HurtWindow&) const = default;
};

}  // namespace rtype::game
