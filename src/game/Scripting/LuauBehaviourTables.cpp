#include "LuauBehaviourTables.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <ratio>
#include <span>
#include <string>
#include <string_view>
#include "ActionKey.hpp"
#include "BehaviourView.hpp"
#include "CollisionBox.hpp"
#include "Direction.hpp"
#include "DirectionConstants.hpp"
#include "EnemyPhase.hpp"
#include "GameScriptingConstants.hpp"
#include "Intent.hpp"
#include "Position.hpp"
#include "TimeConstants.hpp"
#include "lua.h"

namespace rtype::game {

namespace {

double millisecondsOf(engine::Duration duration) {
  return std::chrono::duration<double, std::milli>(duration).count();
}

void setNumber(lua_State* state, const char* key, double value) {
  lua_pushnumber(state, value);
  lua_setfield(state, -2, key);
}

void setFlag(lua_State* state, const char* key, bool flag) {
  lua_pushboolean(state, flag ? 1 : 0);
  lua_setfield(state, -2, key);
}

void setPoint(lua_State* state, const char* key, Position point) {
  lua_createtable(state, 0, 2);
  setNumber(state, POINT_X_KEY, point.x);
  setNumber(state, POINT_Y_KEY, point.y);
  lua_setfield(state, -2, key);
}

void setSettings(lua_State* state, const BehaviourView& view,
                 std::span<const std::string> names) {
  lua_createtable(state, 0, static_cast<int>(names.size()));
  const std::size_t count = std::min(names.size(), view.settings.size());
  for (std::size_t index = 0; index < count; ++index) {
    setNumber(state, names[index].c_str(),
              static_cast<double>(view.settings[index]));
  }
  lua_setfield(state, -2, VIEW_SETTINGS_KEY);
}

void setAction(lua_State* state, const ActionKey& action) {
  lua_createtable(state, 0, 2);
  setNumber(state, ACTION_ATTACK_KEY, action.attack);
  const std::string_view phase =
      PHASE_NAMES.at(static_cast<std::size_t>(action.phase));
  lua_pushlstring(state, phase.data(), phase.size());
  lua_setfield(state, -2, ACTION_PHASE_KEY);
  lua_setfield(state, -2, VIEW_ACTION_KEY);
}

std::optional<double> numberAt(lua_State* state, int table, const char* key) {
  std::optional<double> number;
  if (lua_rawgetfield(state, table, key) == LUA_TNUMBER) {
    number = lua_tonumber(state, -1);
  }
  lua_pop(state, 1);
  return number;
}

bool flagAt(lua_State* state, int table, const char* key) {
  lua_rawgetfield(state, table, key);
  const bool flag = lua_isboolean(state, -1) && lua_toboolean(state, -1) != 0;
  lua_pop(state, 1);
  return flag;
}

std::optional<Position> pointAt(lua_State* state, int table, const char* key) {
  std::optional<Position> point;
  if (lua_rawgetfield(state, table, key) == LUA_TTABLE) {
    const int found = lua_absindex(state, -1);
    const std::optional<double> across = numberAt(state, found, POINT_X_KEY);
    const std::optional<double> down = numberAt(state, found, POINT_Y_KEY);
    if (across.has_value() && down.has_value()) {
      point = Position{.x = static_cast<float>(*across),
                       .y = static_cast<float>(*down)};
    }
  }
  lua_pop(state, 1);
  return point;
}

float shareOf(double value) {
  if (std::isnan(value)) {
    return 0.0F;
  }
  return static_cast<float>(
      std::clamp(value, -double{FULL_STEERING}, double{FULL_STEERING}));
}

std::optional<Position> finitePointAt(lua_State* state, int table,
                                      const char* key) {
  const std::optional<Position> point = pointAt(state, table, key);
  if (point.has_value() &&
      (!std::isfinite(point->x) || !std::isfinite(point->y))) {
    return std::nullopt;
  }
  return point;
}

std::optional<EnemyPhase> phaseNamed(std::string_view name) {
  for (std::size_t index = 0; index < PHASE_NAMES.size(); ++index) {
    if (PHASE_NAMES.at(index) == name) {
      return static_cast<EnemyPhase>(index);
    }
  }
  return std::nullopt;
}

std::optional<EnemyPhase> phaseAt(lua_State* state, int table) {
  std::optional<EnemyPhase> phase;
  if (lua_rawgetfield(state, table, ACTION_PHASE_KEY) == LUA_TSTRING) {
    phase = phaseNamed(lua_tostring(state, -1));
  }
  lua_pop(state, 1);
  return phase;
}

std::optional<ActionKey> actionAt(lua_State* state, int table) {
  std::optional<ActionKey> action;
  if (lua_rawgetfield(state, table, INTENT_STARTS_ACTION_KEY) == LUA_TTABLE) {
    const int found = lua_absindex(state, -1);
    const std::optional<double> attack =
        numberAt(state, found, ACTION_ATTACK_KEY);
    const std::optional<EnemyPhase> phase = phaseAt(state, found);
    const bool whole = attack.has_value() && *attack >= 0 &&
                       *attack <= double{HIGHEST_ATTACK_NUMBER} &&
                       std::floor(*attack) == *attack;
    if (whole && phase.has_value()) {
      action = ActionKey{.attack = static_cast<std::uint8_t>(*attack),
                         .phase = *phase};
    }
  }
  lua_pop(state, 1);
  return action;
}

bool isPositiveFinite(double value) {
  return std::isfinite(value) && value > 0.0;
}

std::optional<CollisionBox> boxAt(lua_State* state, int table) {
  std::optional<CollisionBox> box;
  if (lua_rawgetfield(state, table, INTENT_COLLISION_BOX_KEY) == LUA_TTABLE) {
    const int found = lua_absindex(state, -1);
    const std::optional<double> width = numberAt(state, found, BOX_WIDTH_KEY);
    const std::optional<double> height = numberAt(state, found, BOX_HEIGHT_KEY);
    if (width.has_value() && height.has_value() && isPositiveFinite(*width) &&
        isPositiveFinite(*height)) {
      box = CollisionBox{.width = static_cast<float>(*width),
                         .height = static_cast<float>(*height)};
    }
  }
  lua_pop(state, 1);
  return box;
}

std::optional<float> topSpeedAt(lua_State* state, int table) {
  const std::optional<double> speed =
      numberAt(state, table, INTENT_TOP_SPEED_KEY);
  if (!speed.has_value() || !isPositiveFinite(*speed)) {
    return std::nullopt;
  }
  return static_cast<float>(*speed);
}

}  // namespace

void pushViewTable(lua_State* state, const BehaviourView& view,
                   std::span<const std::string> settingNames) {
  lua_createtable(state, 0, 0);
  setNumber(state, VIEW_ID_KEY, static_cast<double>(view.networkId));
  setPoint(state, VIEW_POSITION_KEY, view.position);
  setPoint(state, VIEW_ORIGIN_KEY, view.origin);
  setNumber(state, VIEW_AGE_KEY, millisecondsOf(view.age));
  setNumber(state, VIEW_ELAPSED_KEY, millisecondsOf(view.elapsed));
  setNumber(state, VIEW_TOP_SPEED_KEY, view.topSpeed);
  setSettings(state, view, settingNames);
  if (view.action.has_value()) {
    setAction(state, *view.action);
  }
  setNumber(state, VIEW_ACTION_AGE_KEY, millisecondsOf(view.actionAge));
  if (view.nearestShip.has_value()) {
    setPoint(state, VIEW_NEAREST_SHIP_KEY, *view.nearestShip);
  }
  setFlag(state, VIEW_WOUNDED_KEY, view.wounded);
  setNumber(state, VIEW_HEALTH_KEY, view.healthShare);
  setFlag(state, VIEW_ENRAGED_KEY, view.enraged);
  setNumber(state, VIEW_ACTIONS_ENDED_KEY,
            static_cast<double>(view.actionsEnded));
}

Intent intentFromTable(lua_State* state, int index) {
  Intent intent{.steered = true};
  if (!lua_istable(state, index)) {
    return intent;
  }
  const int table = lua_absindex(state, index);
  const std::optional<Position> direction =
      pointAt(state, table, INTENT_DIRECTION_KEY);
  if (direction.has_value()) {
    intent.direction =
        Direction{.x = shareOf(direction->x), .y = shareOf(direction->y)};
  }
  intent.shooting = flagAt(state, table, INTENT_SHOOTING_KEY);
  intent.startsAction = actionAt(state, table);
  intent.endsAction = flagAt(state, table, INTENT_ENDS_ACTION_KEY);
  intent.jumpsTo = finitePointAt(state, table, INTENT_JUMPS_TO_KEY);
  intent.resizesTo = boxAt(state, table);
  intent.topSpeedTo = topSpeedAt(state, table);
  return intent;
}

}  // namespace rtype::game
