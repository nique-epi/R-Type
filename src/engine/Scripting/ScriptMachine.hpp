#pragma once

#include <cstddef>
#include <deque>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include "ScriptBudget.hpp"
#include "ScriptFunction.hpp"
#include "ScriptLimits.hpp"
#include "ScriptSource.hpp"
#include "ScriptStepEnd.hpp"

struct lua_State;

namespace rtype::engine {

/**
 * @brief A Luau virtual machine that runs untrusted scripts: they reach no
 * file, process or library of the system, each runs with globals of its own,
 * and they share a memory limit, a time budget per call and per step, and a
 * time budget per tick for the steps together.
 *
 * The program exposes its functions with expose() before the first run();
 * the first run() seals the machine with the sandbox of Luau: the standard
 * libraries and the exposed tables become read-only. A call that passes its
 * budget is stopped at the next loop or call of the script, and fails.
 * Neither copied nor moved: the budget is reached through its address.
 */
class ScriptMachine {
 public:
  /** @throws ScriptException when Luau cannot create the machine. */
  explicit ScriptMachine(ScriptLimits limits);
  ScriptMachine(const ScriptMachine&) = delete;
  ScriptMachine& operator=(const ScriptMachine&) = delete;
  ScriptMachine(ScriptMachine&&) = delete;
  ScriptMachine& operator=(ScriptMachine&&) = delete;
  ~ScriptMachine();

  /**
   * @brief Exposes @p functions to every script as the read-only table
   * @p table, each with @p context as its first upvalue.
   * @throws ScriptException once the machine is sealed.
   */
  void expose(std::string_view table, std::span<const ScriptFunction> functions,
              void* context);

  /**
   * @brief Compiles @p source and runs it once, in a thread of its own whose
   * globals no other script sees.
   * @throws ScriptException, holding the name of the source and the line, when
   * it does not compile, fails, holds too much memory or runs past its budget.
   */
  void run(const ScriptSource& source);

  /**
   * @brief Calls the function under @p arguments values on the stack of
   * state(), leaving @p results values in their place, within the budget.
   * @throws ScriptException when the call fails, as run() does.
   */
  void call(int arguments, int results);

  /** @brief Gives the steps of a new tick the whole of their budget, and
   * collects the garbage at once when it fills most of the memory limit. */
  void beginTick();

  /** @returns Whether a step may start in this tick. */
  [[nodiscard]] bool hasTickBudget() const;

  /**
   * @brief Starts or resumes @p thread with @p arguments values on its stack,
   * within the budget of a step, which counts in the budget of the tick. What
   * the thread returned or yielded is left on its stack.
   * @throws ScriptException when the step fails, as run() does.
   */
  ScriptStepEnd resume(lua_State* thread, int arguments);

  /** @returns The state functions are pushed and read on, for call(). */
  [[nodiscard]] lua_State* state() const;

  /** @returns The memory the scripts hold, in bytes. */
  [[nodiscard]] std::size_t memoryUsed() const;

 private:
  void seal();
  void callOn(lua_State* thread, int arguments, int results);

  ScriptBudget budget_;
  /** The names of the exposed functions: Luau keeps the debug name of a C
   * function by its address and reads it for an error message, so each name
   * lives as long as the machine. Declared before state_, so it outlives it. */
  std::deque<std::string> exposedNames_;
  std::unique_ptr<lua_State, void (*)(lua_State*)> state_;
  bool sealed_{false};
};

}  // namespace rtype::engine
