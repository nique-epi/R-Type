#include "BehaviourScripts.hpp"
#include <array>
#include <cmath>
#include <cstdint>
#include <exception>
#include <limits>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <utility>
#include <vector>
#include "BehaviourRegistry.hpp"
#include "BuiltInBehaviours.hpp"
#include "GameScriptingConstants.hpp"
#include "IEntityOrders.hpp"
#include "LuauBehaviour.hpp"
#include "LuauBehaviourParts.hpp"
#include "LuauEventBindings.hpp"
#include "LuauResumeContext.hpp"
#include "LuauSetting.hpp"
#include "Position.hpp"
#include "ScriptFailureReport.hpp"
#include "ScriptFunction.hpp"
#include "ScriptLimits.hpp"
#include "ScriptSource.hpp"
#include "ScriptWait.hpp"
#include "WholeNumberRange.hpp"
#include "lua.h"
#include "lualib.h"

namespace rtype::game {

namespace {

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
LuauSetting settingOnTop(lua_State* state) {
  luaL_checktype(state, -1, LUA_TTABLE);
  lua_getfield(state, -1, SETTING_NAME_KEY);
  std::string name = luaL_checkstring(state, -1);
  lua_pop(state, 1);
  return LuauSetting{.name = std::move(name),
                     .range = engine::WholeNumberRange{
                         .lowest = wholeNumberOf(state, SETTING_LOWEST_KEY),
                         .highest = wholeNumberOf(state, SETTING_HIGHEST_KEY)}};
}

/** @returns The settings of the array at REGISTER_SETTINGS_ARGUMENT, in its
 * order. */
std::vector<LuauSetting> settingsOf(lua_State* state) {
  luaL_checktype(state, REGISTER_SETTINGS_ARGUMENT, LUA_TTABLE);
  std::vector<LuauSetting> settings;
  const int count = lua_objlen(state, REGISTER_SETTINGS_ARGUMENT);
  for (int index = 1; index <= count; ++index) {
    lua_rawgeti(state, REGISTER_SETTINGS_ARGUMENT, index);
    settings.push_back(settingOnTop(state));
    lua_pop(state, 1);
  }
  return settings;
}

/** @returns The BehaviourScripts in the upvalue of the running function. */
BehaviourScripts& scriptsOf(lua_State* state) {
  return *static_cast<BehaviourScripts*>(
      lua_touserdata(state, lua_upvalueindex(1)));
}

/** @brief Raises @p message as an error of the script. */
[[noreturn]] void raise(lua_State* state, const std::string& message) {
  lua_pushlstring(state, message.data(), message.size());
  lua_error(state);
}

/** @returns The ticks a wait of the milliseconds at its argument lasts, the
 * nearest whole number from 1; raises an error when it is no duration. */
std::uint32_t waitTicksOf(lua_State* state) {
  const std::optional<std::uint32_t> ticks = engine::scriptWaitTicks(
      luaL_checknumber(state, WAIT_MILLISECONDS_ARGUMENT));
  if (!ticks.has_value()) {
    raise(state, "a wait is a finite number of milliseconds, from 0");
  }
  return *ticks;
}

/** @brief `behaviours.wait(ms, intent)`: yields @p intent to the step in
 * progress, which only the thread of a body may do. */
int waitOfBody(lua_State* state) {
  LuauResumeContext& context = scriptsOf(state).resumeContext();
  if (context.resumedThread != state) {
    raise(state, "behaviours.wait only runs in the function of a body");
  }
  context.waitTicks = waitTicksOf(state);
  lua_settop(state, WAIT_INTENT_ARGUMENT);
  return lua_yield(state, WAIT_YIELDED_VALUES);
}

/** @returns What carries out the bodies the scripts of @p state order;
 * raises an error outside a game. */
IEntityOrders& ordersOf(lua_State* state) {
  IEntityOrders* const orders = scriptsOf(state).orders();
  if (orders == nullptr) {
    raise(state, "bodies are only created and destroyed during a game");
  }
  return *orders;
}

/** @returns The point at the argument SPAWN_POSITION_ARGUMENT; raises an error
 * when it is not a table of two finite numbers. */
Position spawnPositionOf(lua_State* state) {
  luaL_checktype(state, SPAWN_POSITION_ARGUMENT, LUA_TTABLE);
  lua_rawgetfield(state, SPAWN_POSITION_ARGUMENT, POINT_X_KEY);
  lua_rawgetfield(state, SPAWN_POSITION_ARGUMENT, POINT_Y_KEY);
  const auto across = static_cast<float>(lua_tonumber(state, -2));
  const auto down = static_cast<float>(lua_tonumber(state, -1));
  if (lua_type(state, -2) != LUA_TNUMBER ||
      lua_type(state, -1) != LUA_TNUMBER || !std::isfinite(across) ||
      !std::isfinite(down)) {
    raise(state, "a position is a table of two finite numbers, x and y");
  }
  lua_pop(state, 2);
  return Position{.x = across, .y = down};
}

/** @brief `behaviours.spawn(type, position)`: returns the network id of the
 * body, which appears once the bodies of this tick were steered. */
int spawnBody(lua_State* state) {
  IEntityOrders& orders = ordersOf(state);
  const char* const type = luaL_checkstring(state, SPAWN_TYPE_ARGUMENT);
  const Position position = spawnPositionOf(state);
  std::uint32_t networkId = 0;
  std::string refusal;
  try {
    networkId = orders.spawn(type, position);
  } catch (const std::exception& error) {
    refusal = error.what();
  }
  if (!refusal.empty()) {
    raise(state, refusal);
  }
  lua_pushnumber(state, static_cast<double>(networkId));
  return 1;
}

/** @returns The network id at the argument DESTROY_ID_ARGUMENT; raises an
 * error when it is not a whole number a network id can be. */
std::uint32_t destroyedIdOf(lua_State* state) {
  const double identifier = luaL_checknumber(state, DESTROY_ID_ARGUMENT);
  if (std::floor(identifier) != identifier || identifier < 0.0 ||
      identifier > double{std::numeric_limits<std::uint32_t>::max()}) {
    raise(state, "the id of a body is a whole number");
  }
  return static_cast<std::uint32_t>(identifier);
}

/** @brief `behaviours.destroy(id)`: removes the body once the bodies of this
 * tick were steered. */
int destroyBody(lua_State* state) {
  IEntityOrders& orders = ordersOf(state);
  const std::uint32_t networkId = destroyedIdOf(state);
  std::string refusal;
  try {
    orders.destroy(networkId);
  } catch (const std::exception& error) {
    refusal = error.what();
  }
  if (!refusal.empty()) {
    raise(state, refusal);
  }
  return 0;
}

/** @brief `behaviours.register(name, settings, steer)`: keeps a behaviour in
 * the BehaviourScripts of its upvalue. */
int registerBehaviour(lua_State* state) {
  BehaviourScripts& scripts = scriptsOf(state);
  std::string name = luaL_checkstring(state, REGISTER_NAME_ARGUMENT);
  std::vector<LuauSetting> settings = settingsOf(state);
  luaL_checktype(state, REGISTER_STEER_ARGUMENT, LUA_TFUNCTION);
  const int steer = lua_ref(state, REGISTER_STEER_ARGUMENT);
  scripts.declare(LuauBehaviourParts{.machine = scripts.machine(),
                                     .name = std::move(name),
                                     .settings = std::move(settings),
                                     .steer = steer,
                                     .reportFailure = scripts.reportFailure(),
                                     .resumeContext = scripts.resumeContext(),
                                     .eventBindings = scripts.eventBindings()});
  return 0;
}

}  // namespace

BehaviourScripts::BehaviourScripts(ScriptFailureReport reportFailure)
    : machine_(engine::ScriptLimits{}),
      reportFailure_(std::move(reportFailure)),
      eventBindings_(machine_) {
  const std::array functions{
      engine::ScriptFunction{.name = REGISTER_FUNCTION_NAME,
                             .function = registerBehaviour},
      engine::ScriptFunction{.name = WAIT_FUNCTION_NAME,
                             .function = waitOfBody},
      engine::ScriptFunction{.name = SPAWN_FUNCTION_NAME,
                             .function = spawnBody},
      engine::ScriptFunction{.name = DESTROY_FUNCTION_NAME,
                             .function = destroyBody}};
  machine_.expose(BEHAVIOURS_TABLE, functions, this);
}

void BehaviourScripts::run(std::span<const engine::ScriptSource> sources) {
  for (const engine::ScriptSource& source : sources) {
    machine_.run(source);
  }
}

void BehaviourScripts::declare(LuauBehaviourParts parts) {
  declared_.push_back(std::make_unique<LuauBehaviour>(std::move(parts)));
}

engine::ScriptMachine& BehaviourScripts::machine() { return machine_; }

void BehaviourScripts::attachGame(IEntityOrders& orders,
                                  engine::ComponentRegistry& components) {
  resumeContext_.orders = &orders;
  resumeContext_.steppedBody.components = &components;
}

void BehaviourScripts::detachGame(const IEntityOrders& orders) noexcept {
  if (resumeContext_.orders == &orders) {
    resumeContext_.orders = nullptr;
    resumeContext_.steppedBody.components = nullptr;
  }
}

IEntityOrders* BehaviourScripts::orders() const {
  return resumeContext_.orders;
}

LuauResumeContext& BehaviourScripts::resumeContext() { return resumeContext_; }

LuauEventBindings& BehaviourScripts::eventBindings() { return eventBindings_; }

const ScriptFailureReport& BehaviourScripts::reportFailure() const {
  return reportFailure_;
}

BehaviourRegistry BehaviourScripts::behaviours() {
  BehaviourRegistry registry = builtInBehaviours();
  for (std::unique_ptr<LuauBehaviour>& behaviour : declared_) {
    registry.add(std::move(behaviour));
  }
  declared_.clear();
  return registry;
}

}  // namespace rtype::game
