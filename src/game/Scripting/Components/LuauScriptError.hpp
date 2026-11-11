#pragma once

#include <string_view>

struct lua_State;

namespace rtype::game {

/** @brief Raises @p message as an error of the script, after the place of the
 * line of the script that called the running C function or metamethod. */
[[noreturn]] void raiseAtScriptLine(lua_State* state, std::string_view message);

}  // namespace rtype::game
