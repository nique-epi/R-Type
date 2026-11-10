#pragma once

struct lua_State;

namespace rtype::engine {

/**
 * @brief A thread of a script machine, which runs a function that can stop
 * and be resumed later: what it holds is kept alive as long as this object.
 *
 * Build it inside runProtected(), since creating it allocates. Destroying it
 * lets the collector of Luau take the thread, whatever the thread was doing;
 * it allocates nothing and must happen while no script runs and while the
 * machine is alive. Neither copied nor moved: its destructor gives its
 * reference back once.
 */
class LuauThread {
 public:
  /** @param machine The main state of the machine that owns the thread. */
  explicit LuauThread(lua_State* machine);
  LuauThread(const LuauThread&) = delete;
  LuauThread& operator=(const LuauThread&) = delete;
  LuauThread(LuauThread&&) = delete;
  LuauThread& operator=(LuauThread&&) = delete;
  ~LuauThread();

  /** @returns The state of the thread, for ScriptMachine::resume(). */
  [[nodiscard]] lua_State* state() const;

 private:
  lua_State* machine_;
  lua_State* state_;
  int reference_;
};

}  // namespace rtype::engine
