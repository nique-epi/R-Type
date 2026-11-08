#include "BehaviourScripts.hpp"
#include <array>
#include <cmath>
#include <cstdint>
#include <memory>
#include <span>
#include <string>
#include <utility>
#include <vector>
#include "BehaviourRegistry.hpp"
#include "BuiltInBehaviours.hpp"
#include "GameScriptingConstants.hpp"
#include "ScriptFailureReport.hpp"
#include "ScriptFunction.hpp"
#include "ScriptLimits.hpp"
#include "ScriptSource.hpp"
#include "ScriptedBehaviour.hpp"
#include "ScriptedBehaviourParts.hpp"
#include "ScriptedSetting.hpp"
#include "WholeNumberRange.hpp"
#include "lua.h"
#include "lualib.h"

namespace rtype::game {

namespace {

constexpr int NAME_ARGUMENT = 1;
constexpr int SETTINGS_ARGUMENT = 2;
constexpr int STEER_ARGUMENT = 3;

/** @returns The whole number @p key of the setting table on top of @p state;
 * raises an error of the script when it is not one. */
std::int64_t wholeNumberOf(lua_State* state, const char* key) {
  lua_getfield(state, -1, key);
  const double value = lua_tonumber(state, -1);
  if (lua_type(state, -1) != LUA_TNUMBER || std::floor(value) != value) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg,hicpp-vararg)
    luaL_errorL(state, "the %s of a setting must be a whole number", key);
  }
  lua_pop(state, 1);
  return static_cast<std::int64_t>(value);
}

/** @returns The setting described by the table on top of @p state. */
ScriptedSetting settingOnTop(lua_State* state) {
  luaL_checktype(state, -1, LUA_TTABLE);
  lua_getfield(state, -1, SETTING_NAME_KEY);
  std::string name = luaL_checkstring(state, -1);
  lua_pop(state, 1);
  return ScriptedSetting{
      .name = std::move(name),
      .range = engine::WholeNumberRange{
          .lowest = wholeNumberOf(state, SETTING_LOWEST_KEY),
          .highest = wholeNumberOf(state, SETTING_HIGHEST_KEY)}};
}

/** @returns The settings of the array at SETTINGS_ARGUMENT, in its order. */
std::vector<ScriptedSetting> settingsOf(lua_State* state) {
  luaL_checktype(state, SETTINGS_ARGUMENT, LUA_TTABLE);
  std::vector<ScriptedSetting> settings;
  const int count = lua_objlen(state, SETTINGS_ARGUMENT);
  for (int index = 1; index <= count; ++index) {
    lua_rawgeti(state, SETTINGS_ARGUMENT, index);
    settings.push_back(settingOnTop(state));
    lua_pop(state, 1);
  }
  return settings;
}

/** @brief `behaviours.register(name, settings, steer)`: keeps a behaviour in
 * the BehaviourScripts of its upvalue. */
int registerBehaviour(lua_State* state) {
  BehaviourScripts& scripts = *static_cast<BehaviourScripts*>(
      lua_touserdata(state, lua_upvalueindex(1)));
  std::string name = luaL_checkstring(state, NAME_ARGUMENT);
  std::vector<ScriptedSetting> settings = settingsOf(state);
  luaL_checktype(state, STEER_ARGUMENT, LUA_TFUNCTION);
  const int steer = lua_ref(state, STEER_ARGUMENT);
  scripts.declare(
      ScriptedBehaviourParts{.machine = scripts.machine(),
                             .name = std::move(name),
                             .settings = std::move(settings),
                             .steer = steer,
                             .reportFailure = scripts.reportFailure()});
  return 0;
}

}  // namespace

BehaviourScripts::BehaviourScripts(ScriptFailureReport reportFailure)
    : machine_(engine::ScriptLimits{}),
      reportFailure_(std::move(reportFailure)) {
  const std::array functions{engine::ScriptFunction{
      .name = REGISTER_FUNCTION_NAME, .function = registerBehaviour}};
  machine_.expose(BEHAVIOURS_TABLE, functions, this);
}

void BehaviourScripts::run(std::span<const engine::ScriptSource> sources) {
  for (const engine::ScriptSource& source : sources) {
    machine_.run(source);
  }
}

void BehaviourScripts::declare(ScriptedBehaviourParts parts) {
  declared_.push_back(std::make_unique<ScriptedBehaviour>(std::move(parts)));
}

engine::ScriptMachine& BehaviourScripts::machine() { return machine_; }

const ScriptFailureReport& BehaviourScripts::reportFailure() const {
  return reportFailure_;
}

BehaviourRegistry BehaviourScripts::behaviours() {
  BehaviourRegistry registry = builtInBehaviours();
  for (std::unique_ptr<ScriptedBehaviour>& behaviour : declared_) {
    registry.add(std::move(behaviour));
  }
  declared_.clear();
  return registry;
}

}  // namespace rtype::game
