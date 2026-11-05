#pragma once

#include <span>
#include <string_view>
#include "BehaviourSetting.hpp"
#include "BehaviourView.hpp"
#include "Intent.hpp"

namespace rtype::game {

/**
 * @brief A pilot that is not a player: decides, once per tick, what its body
 * wants.
 *
 * It reads only what BehaviourView gives of its own body, the values its sheet
 * gives its settings included, and returns the whole Intent of that body; it
 * keeps no state, so one behaviour pilots every body that names it, whatever
 * their settings. PilotSystem calls it; BehaviourRegistry finds it by its
 * name.
 */
class IBehaviour {
 public:
  virtual ~IBehaviour() = default;

  /** @returns The name a body is given this behaviour by: lowercase letters,
   * digits and underscores, unique among the behaviours of a registry. */
  [[nodiscard]] virtual std::string_view name() const = 0;

  /** @returns The settings a sheet gives this behaviour, in the order
   * BehaviourView::settings holds their values; empty for a behaviour that
   * reads none. The array outlives the behaviour. */
  [[nodiscard]] virtual std::span<const BehaviourSetting> settings() const = 0;

  /** @returns What the body @p view describes wants at the end of this tick.
   */
  [[nodiscard]] virtual Intent steer(const BehaviourView& view) const = 0;
};

}  // namespace rtype::game
