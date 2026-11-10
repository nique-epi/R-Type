#include "LuauThread.hpp"
#include "lua.h"

namespace rtype::engine {

namespace {

/** @returns The reference that anchors the value on top of @p state, popped. */
int anchorTop(lua_State* state) {
  const int reference = lua_ref(state, -1);
  lua_pop(state, 1);
  return reference;
}

}  // namespace

LuauThread::LuauThread(lua_State* machine)
    : machine_(machine),
      state_(lua_newthread(machine)),
      reference_(anchorTop(machine)) {}

LuauThread::~LuauThread() { lua_unref(machine_, reference_); }

lua_State* LuauThread::state() const { return state_; }

}  // namespace rtype::engine
