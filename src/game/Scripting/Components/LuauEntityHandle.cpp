#include "LuauEntityHandle.hpp"
#include <string_view>
#include "LuauComponentConstants.hpp"
#include "LuauComponentFields.hpp"
#include "LuauHandleSlot.hpp"
#include "LuauScriptError.hpp"
#include "LuauSteppedBody.hpp"
#include "lua.h"
#include "lualib.h"

namespace rtype::game {

namespace {

/** @returns The body of the step in progress, in the upvalue of the running
 * metamethod. */
const LuauSteppedBody& steppedBodyOf(lua_State* state) {
  return *static_cast<const LuauSteppedBody*>(
      lua_touserdata(state, lua_upvalueindex(1)));
}

/** @returns The slot of the handle a metamethod runs for; raises an error
 * once its body ended. */
const LuauHandleSlot& liveSlotOf(lua_State* state) {
  const auto* const slot = static_cast<const LuauHandleSlot*>(
      lua_touserdata(state, HANDLE_ARGUMENT));
  if (slot == nullptr || !slot->alive) {
    raiseAtScriptLine(state, GONE_BODY_ERROR);
  }
  return *slot;
}

/** @returns The body that steps; raises an error unless @p slot is its
 * handle. */
const LuauSteppedBody& ownStepOf(lua_State* state, const LuauHandleSlot& slot) {
  const LuauSteppedBody& body = steppedBodyOf(state);
  if (body.handle != &slot) {
    raiseAtScriptLine(state, OTHER_STEP_ERROR);
  }
  return body;
}

/** @brief Pushes the field @p field of the hidden table of @p slot. */
void pushHiddenField(lua_State* state, const LuauHandleSlot& slot,
                     const char* field) {
  lua_getref(state, slot.hiddenTable);
  lua_rawgetfield(state, -1, field);
  lua_remove(state, -2);
}

/** @brief `e.key`: a component of the body during its step, its data, or a
 * field of the view of its last step. */
int readHandleField(lua_State* state) {
  const LuauHandleSlot& slot = liveSlotOf(state);
  const char* const key = luaL_checkstring(state, FIELD_ARGUMENT);
  if (isComponentKey(key)) {
    const LuauSteppedBody& body = ownStepOf(state, slot);
    if (body.components != nullptr) {
      pushComponentField(state, body, key);
      return 1;
    }
  }
  if (std::string_view{key} == SCRIPT_DATA_KEY) {
    pushHiddenField(state, slot, SCRIPT_DATA_KEY);
    return 1;
  }
  pushHiddenField(state, slot, HANDLE_VIEW_FIELD);
  lua_rawgetfield(state, -1, key);
  lua_remove(state, -2);
  return 1;
}

/** @brief `e.key = value`: writes a component of the body during its step
 * in a game; raises an error for any other field. */
int writeHandleField(lua_State* state) {
  const LuauHandleSlot& slot = liveSlotOf(state);
  const std::string_view key = luaL_checkstring(state, FIELD_ARGUMENT);
  if (key == SCRIPT_DATA_KEY) {
    raiseAtScriptLine(state, DATA_REPLACED_ERROR);
  }
  if (!isComponentKey(key)) {
    raiseAtScriptLine(state, fieldText(key, READ_ONLY_FIELD_ERROR));
  }
  const LuauSteppedBody& body = ownStepOf(state, slot);
  if (body.components == nullptr) {
    raiseAtScriptLine(state, OUTSIDE_GAME_WRITE_ERROR);
  }
  writeComponentField(state, body, key);
  return 0;
}

/** @brief Pushes the metatable of the handles of @p state, made the first
 * time, its metamethods closing over @p steppedBody. */
void pushHandleMetatable(lua_State* state, LuauSteppedBody& steppedBody) {
  if (luaL_newmetatable(state, ENTITY_HANDLE_METATABLE) == 0) {
    return;
  }
  lua_pushlightuserdata(state, &steppedBody);
  lua_pushcclosure(state, readHandleField, INDEX_METAMETHOD, 1);
  lua_setfield(state, -2, INDEX_METAMETHOD);
  lua_pushlightuserdata(state, &steppedBody);
  lua_pushcclosure(state, writeHandleField, NEW_INDEX_METAMETHOD, 1);
  lua_setfield(state, -2, NEW_INDEX_METAMETHOD);
  lua_pushstring(state, LOCKED_METATABLE_TEXT);
  lua_setfield(state, -2, METATABLE_FIELD);
}

/** @returns The reference of a new hidden table of a handle, holding an
 * empty `data`. */
int referenceNewHiddenTable(lua_State* machine) {
  lua_createtable(machine, 0, 2);
  lua_createtable(machine, 0, 0);
  lua_setfield(machine, -2, SCRIPT_DATA_KEY);
  const int reference = lua_ref(machine, -1);
  lua_pop(machine, 1);
  return reference;
}

/** @returns The slot of a new handle userdata, pushed on @p machine, whose
 * hidden table is @p hiddenTable. */
LuauHandleSlot* pushNewSlot(lua_State* machine, int hiddenTable) {
  auto* const slot = static_cast<LuauHandleSlot*>(
      lua_newuserdata(machine, sizeof(LuauHandleSlot)));
  *slot = LuauHandleSlot{.hiddenTable = hiddenTable, .alive = true};
  return slot;
}

/** @returns The reference of the handle on top of @p machine, which it pops,
 * given the metatable of the handles first. */
int referenceHandleOnTop(lua_State* machine, LuauSteppedBody& steppedBody) {
  pushHandleMetatable(machine, steppedBody);
  lua_setmetatable(machine, -2);
  const int reference = lua_ref(machine, -1);
  lua_pop(machine, 1);
  return reference;
}

}  // namespace

LuauEntityHandle::LuauEntityHandle(lua_State* machine,
                                   LuauSteppedBody& steppedBody)
    : machine_(machine),
      slot_(pushNewSlot(machine, referenceNewHiddenTable(machine))),
      handle_(referenceHandleOnTop(machine, steppedBody)) {}

LuauEntityHandle::~LuauEntityHandle() {
  slot_->alive = false;
  lua_unref(machine_, handle_);
  lua_unref(machine_, slot_->hiddenTable);
}

void LuauEntityHandle::pushWithView() const {
  lua_getref(machine_, slot_->hiddenTable);
  lua_insert(machine_, -2);
  lua_setfield(machine_, -2, HANDLE_VIEW_FIELD);
  lua_pop(machine_, 1);
  lua_getref(machine_, handle_);
}

const LuauHandleSlot* LuauEntityHandle::slot() const { return slot_; }

}  // namespace rtype::game
