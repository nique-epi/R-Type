#pragma once

#include <cstddef>
#include <string_view>

namespace rtype::game {

/**
 * @brief The bosses a level brings in: a stage names one, the players see the
 * warning of its arrival, and the boss enters once the warning is over.
 */
class ILevelBosses {
 public:
  virtual ~ILevelBosses() = default;

  /** @returns Whether @p name is the sheet of a boss, one with a
   * `[gameplay.boss]` table. */
  [[nodiscard]] virtual bool isBoss(std::string_view name) const = 0;

  /** @brief Warns the players that the boss @p name arrives, with
   * @p healthPercent percent of the health of its sheet (1 to 100); it enters
   * once the warning is over. @p name must be a boss (see isBoss()). */
  virtual void announce(std::string_view name, int healthPercent) = 0;

  /** @returns How many bosses were announced and have not entered yet. */
  [[nodiscard]] virtual std::size_t pending() const = 0;
};

}  // namespace rtype::game
