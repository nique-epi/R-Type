#pragma once

#include <string>

struct lua_State;

namespace rtype::engine {

/** @brief Work that calls the API of Luau on a machine: @p run is called with
 * @p context. */
struct ScriptWork {
  void (*run)(void* context){nullptr};
  void* context{nullptr};
};

/**
 * @brief Runs @p work inside a protected call of @p state: outside one, Luau
 * ends the program on the first allocation a machine at its memory limit
 * refuses.
 * @throws ScriptException holding the error @p work raised.
 */
void runProtected(lua_State* state, const ScriptWork& work);

/** @returns The error on top of the stack of @p thread, popped. */
[[nodiscard]] std::string popScriptError(lua_State* thread);

}  // namespace rtype::engine
