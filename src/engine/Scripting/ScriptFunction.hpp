#pragma once

#include <string_view>

struct lua_State;

namespace rtype::engine {

/** @brief A function of the program a script may call: its name in the table
 * it is exposed in, and the function, which finds what it acts on in its
 * first upvalue. */
struct ScriptFunction {
  std::string_view name;
  int (*function)(lua_State*){nullptr};
};

}  // namespace rtype::engine
