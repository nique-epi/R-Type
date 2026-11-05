#pragma once

#include <string_view>
#include "BehaviourView.hpp"
#include "Intent.hpp"

namespace rtype::game {

/**
 * @brief A pilot that is not a player: decides, once per tick, what its body
 * wants.
 *
 * It reads only what BehaviourView gives of its own body and returns the
 * whole Intent of that body; it keeps no state, so one behaviour pilots every
 * body that names it. PilotSystem calls it; BehaviourRegistry finds it by its
 * name.
 */
class IBehaviour {
 public:
  virtual ~IBehaviour() = default;

  /** @returns The name a body is given this behaviour by: lowercase letters,
   * digits and underscores, unique among the behaviours of a registry. */
  [[nodiscard]] virtual std::string_view name() const = 0;

  /** @returns What the body @p view describes wants at the end of this tick.
   */
  [[nodiscard]] virtual Intent steer(const BehaviourView& view) const = 0;
};

}  // namespace rtype::game
