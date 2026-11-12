#pragma once

#include <functional>
#include "IncomingHit.hpp"
#include "ScriptMachine.hpp"

namespace rtype::game {

/**
 * @brief The function the script of one body declared with
 * `behaviours.pre_hit`, which judges each hit of a missile before it is
 * announced. It receives the hit (see pushHitTable()); returning `false`
 * cancels it, and any other value lets it apply.
 *
 * Each judgement runs in a Luau thread of its own, with the budget of a step,
 * counted in the budget of the tick: once the budget of the tick is spent, the
 * hit applies without a call. The handle of the body refuses every key there,
 * as in a function subscribed to an event, since no step of the body runs.
 * Neither copied nor moved: its destructor gives its reference back once, and
 * must run while the machine is alive.
 */
class LuauPreHit {
 public:
  /** @param function The reference of the function in the registry of
   * @p machine, which it takes over. */
  LuauPreHit(engine::ScriptMachine& machine, int function);
  LuauPreHit(const LuauPreHit&) = delete;
  LuauPreHit& operator=(const LuauPreHit&) = delete;
  LuauPreHit(LuauPreHit&&) = delete;
  LuauPreHit& operator=(LuauPreHit&&) = delete;
  ~LuauPreHit();

  /**
   * @returns Whether @p hit applies: false only when the function returned
   * `false`; true without a call once the budget of the tick is spent.
   * @throws engine::ScriptException when the function fails, waits or runs
   * past its budget.
   */
  [[nodiscard]] bool lets(const IncomingHit& hit);

 private:
  std::reference_wrapper<engine::ScriptMachine> machine_;
  int function_;
};

}  // namespace rtype::game
