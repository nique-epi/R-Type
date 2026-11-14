#pragma once

#include <string_view>
#include "AppliedHit.hpp"

namespace rtype::game {

/** @brief What a sheet names in `on_hit`: told of every hit of a missile
 * applied to a body of that sheet, once DamageRules applied it. */
class IHitHandler {
 public:
  virtual ~IHitHandler() = default;

  /** @returns The name a sheet gives it in `on_hit`. */
  [[nodiscard]] virtual std::string_view name() const = 0;

  /** @brief Reacts to @p hit, already applied: it may order bodies, never undo
   * the hit. */
  virtual void onHit(const AppliedHit& hit) = 0;
};

}  // namespace rtype::game
