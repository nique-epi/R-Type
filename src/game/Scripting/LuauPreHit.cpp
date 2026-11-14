#include "LuauPreHit.hpp"
#include "GameScriptingConstants.hpp"
#include "IncomingHit.hpp"
#include "LuauBehaviourTables.hpp"
#include "LuauCallArguments.hpp"
#include "LuauSingleCall.hpp"
#include "ScriptMachine.hpp"
#include "lua.h"

namespace rtype::game {

namespace {

/** @brief Pushes the table of the IncomingHit @p value. */
void pushIncomingHit(lua_State* machine, const void* value) {
  pushHitTable(machine, *static_cast<const IncomingHit*>(value));
}

/** @returns Whether the first value a judgement left on @p thread is
 * `false`. */
bool cancels(lua_State* thread) {
  return lua_gettop(thread) > 0 && lua_type(thread, 1) == LUA_TBOOLEAN &&
         lua_toboolean(thread, 1) == 0;
}

}  // namespace

LuauPreHit::LuauPreHit(engine::ScriptMachine& machine, int function)
    : machine_(machine), function_(function) {}

LuauPreHit::~LuauPreHit() { lua_unref(machine_.get().state(), function_); }

bool LuauPreHit::lets(const IncomingHit& hit) {
  if (!machine_.get().hasTickBudget()) {
    return true;
  }
  LuauSingleCall call{machine_.get(), function_};
  call.run(LuauCallArguments{.push = pushIncomingHit, .value = &hit},
           WAIT_IN_PRE_HIT_MESSAGE);
  return !cancels(call.thread());
}

}  // namespace rtype::game
