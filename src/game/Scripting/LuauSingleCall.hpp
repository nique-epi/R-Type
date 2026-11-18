#pragma once

#include <functional>
#include <optional>
#include <string_view>
#include "LuauCallArguments.hpp"
#include "LuauThread.hpp"
#include "ScriptMachine.hpp"

struct lua_State;

namespace rtype::game {

/**
 * @brief One call of a function of the registry of a machine, on a Luau thread
 * of its own: the call of a pre_hit or an on_hit function. The thread, and
 * what the function returned on it, live as long as this object.
 *
 * The call runs with the budget of a step, counted in the budget of the tick;
 * the caller checks the budget of the tick before. Neither copied nor moved,
 * and destroyed while the machine is alive and no script runs.
 */
class LuauSingleCall {
 public:
  /** @param function The reference of the function in the registry of
   * @p machine, which stays its owner's. */
  LuauSingleCall(engine::ScriptMachine& machine, int function);

  /**
   * @brief Calls the function with @p arguments.
   * @throws engine::ScriptException when it fails, runs past its budget, or
   * waits, then with @p waitRefusal: a function called once may not wait.
   */
  void run(LuauCallArguments arguments, std::string_view waitRefusal);

  /** @returns The thread the function ran on, which holds what it returned;
   * nullptr before run(). */
  [[nodiscard]] lua_State* thread() const;

 private:
  std::reference_wrapper<engine::ScriptMachine> machine_;
  int function_;
  std::optional<engine::LuauThread> thread_;
};

}  // namespace rtype::game
