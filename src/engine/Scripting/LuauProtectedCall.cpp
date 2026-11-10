#include "LuauProtectedCall.hpp"
#include <string>
#include "EngineException.hpp"
#include "lua.h"

namespace rtype::engine {

namespace {

/** @brief Runs the LuauProtectedWork whose address is the first argument. */
int runWork(lua_State* state) {
  const auto& work =
      *static_cast<const LuauProtectedWork*>(lua_touserdata(state, 1));
  work.run(work.context);
  return 0;
}

}  // namespace

void runProtected(lua_State* state, const LuauProtectedWork& work) {
  LuauProtectedWork handed = work;
  if (lua_cpcall(state, runWork, &handed) != 0) {
    throw ScriptException(popScriptError(state));
  }
}

std::string popScriptError(lua_State* thread) {
  const char* const text = lua_tostring(thread, -1);
  std::string message = text == nullptr ? std::string{} : std::string{text};
  lua_pop(thread, 1);
  return message;
}

}  // namespace rtype::engine
