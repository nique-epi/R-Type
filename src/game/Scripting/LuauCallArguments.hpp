#pragma once

struct lua_State;

namespace rtype::game {

/** @brief The argument a LuauSingleCall passes its function: @p push pushes
 * @p value on the machine, as one value. */
struct LuauCallArguments {
  void (*push)(lua_State* machine, const void* value){nullptr};
  const void* value{nullptr};
};

}  // namespace rtype::game
