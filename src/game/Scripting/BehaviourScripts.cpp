#include "BehaviourScripts.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <exception>
#include <limits>
#include <memory>
#include <span>
#include <string>
#include <utility>
#include <vector>
#include "BehaviourRegistry.hpp"
#include "BuiltInBehaviours.hpp"
#include "GameScriptingConstants.hpp"
#include "IBodyOrders.hpp"
#include "LuauBehaviour.hpp"
#include "LuauBehaviourParts.hpp"
#include "LuauSetting.hpp"
#include "LuauStepState.hpp"
#include "Position.hpp"
#include "ScriptFailureReport.hpp"
#include "ScriptFunction.hpp"
#include "ScriptLimits.hpp"
#include "ScriptSource.hpp"
#include "TimeConstants.hpp"
#include "WholeNumberRange.hpp"
#include "lua.h"
#include "lualib.h"

namespace rtype::game {

namespace {

constexpr int NAME_ARGUMENT = 1;
constexpr int SETTINGS_ARGUMENT = 2;
constexpr int STEER_ARGUMENT = 3;
constexpr int WAIT_MILLISECONDS_ARGUMENT = 1;
constexpr int WAIT_INTENT_ARGUMENT = 2;
constexpr int SPAWN_TYPE_ARGUMENT = 1;
constexpr int SPAWN_POSITION_ARGUMENT = 2;
constexpr int DESTROY_ID_ARGUMENT = 1;
constexpr double MILLISECONDS_PER_SECOND = 1000.0;
constexpr int YIELDED_INTENT = 1;

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

/** @returns The settings of the array at SETTINGS_ARGUMENT, in its order. */
std::vector<LuauSetting> settingsOf(lua_State* state) {
  luaL_checktype(state, SETTINGS_ARGUMENT, LUA_TTABLE);
  std::vector<LuauSetting> settings;
  const int count = lua_objlen(state, SETTINGS_ARGUMENT);
  for (int index = 1; index <= count; ++index) {
    lua_rawgeti(state, SETTINGS_ARGUMENT, index);
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
  const double milliseconds =
      luaL_checknumber(state, WAIT_MILLISECONDS_ARGUMENT);
  if (!std::isfinite(milliseconds) || milliseconds < 0.0) {
    raise(state, "a wait is a finite number of milliseconds, from 0");
  }
  const double ticks = std::round(
      milliseconds * static_cast<double>(engine::SIMULATION_TICKS_PER_SECOND) /
      MILLISECONDS_PER_SECOND);
  return static_cast<std::uint32_t>(std::clamp(
      ticks, 1.0, double{std::numeric_limits<std::uint32_t>::max()}));
}

/** @brief `behaviours.wait(ms, intent)`: yields @p intent to the step in
 * progress, which only the thread of a body may do. */
int waitOfBody(lua_State* state) {
  LuauStepState& step = scriptsOf(state).step();
  if (step.stepping != state) {
    raise(state, "behaviours.wait only runs in the function of a body");
  }
  step.waitTicks = waitTicksOf(state);
  lua_settop(state, WAIT_INTENT_ARGUMENT);
  return lua_yield(state, YIELDED_INTENT);
}

/** @returns What carries out the bodies the scripts of @p state order;
 * raises an error outside a game. */
IBodyOrders& ordersOf(lua_State* state) {
  IBodyOrders* const orders = scriptsOf(state).orders();
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
  IBodyOrders& orders = ordersOf(state);
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
  IBodyOrders& orders = ordersOf(state);
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
  std::string name = luaL_checkstring(state, NAME_ARGUMENT);
  std::vector<LuauSetting> settings = settingsOf(state);
  luaL_checktype(state, STEER_ARGUMENT, LUA_TFUNCTION);
  const int steer = lua_ref(state, STEER_ARGUMENT);
  scripts.declare(LuauBehaviourParts{.machine = scripts.machine(),
                                     .name = std::move(name),
                                     .settings = std::move(settings),
                                     .steer = steer,
                                     .reportFailure = scripts.reportFailure(),
                                     .step = scripts.step()});
  return 0;
}

}  // namespace

BehaviourScripts::BehaviourScripts(ScriptFailureReport reportFailure)
    : machine_(engine::ScriptLimits{}),
      reportFailure_(std::move(reportFailure)) {
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

void BehaviourScripts::attachOrders(IBodyOrders& orders) {
  step_.orders = &orders;
}

void BehaviourScripts::detachOrders(const IBodyOrders& orders) noexcept {
  if (step_.orders == &orders) {
    step_.orders = nullptr;
  }
}

IBodyOrders* BehaviourScripts::orders() const { return step_.orders; }

LuauStepState& BehaviourScripts::step() { return step_; }

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
