#pragma once

#include <cstdint>
#include <optional>
#include "Intent.hpp"

namespace rtype::game {

/**
 * @brief The rules of a body that acts in steps separated by waits, whatever
 * runs its steps: it holds what a step asked for the ticks of its wait,
 * applies the one-time requests of that step (an action started or ended, a
 * jump, an effect played) on its first tick only, and stands still for good once stopped.
 */
class HeldIntent {
 public:
  /**
   * @returns What the body wants this tick without a new step: standing still
   * once stopped, or what the last step asked while its wait lasts, one tick
   * of the wait counted. Nothing when a step is due.
   */
  [[nodiscard]] std::optional<Intent> heldThisTick();

  /** @returns What the body wants while a due step cannot run this tick. */
  [[nodiscard]] Intent held() const;

  /**
   * @brief Records a step that asked for @p intent and then a wait of
   * @p waitTicks, this tick included; 0 when the next step is due at the
   * next tick.
   * @returns @p intent, what the body wants on this tick.
   */
  Intent startHolding(const Intent& intent, std::uint32_t waitTicks);

  /** @brief Stops holding for good: the body stands still. */
  void stop();

  /** @returns Whether it stopped. */
  [[nodiscard]] bool stopped() const;

 private:
  std::uint32_t ticksLeft_{0};
  Intent held_{.steered = true};
  bool stopped_{false};
};

}  // namespace rtype::game
