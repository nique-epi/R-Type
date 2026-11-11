#include "LuauScriptError.hpp"
#include <string_view>
#include "lua.h"
#include "lualib.h"

namespace rtype::game {

void raiseAtScriptLine(lua_State* state, std::string_view message) {
  luaL_where(state, 1);
  lua_pushlstring(state, message.data(), message.size());
  lua_concat(state, 2);
  lua_error(state);
}

}  // namespace rtype::game
