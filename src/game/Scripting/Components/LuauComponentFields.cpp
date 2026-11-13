#include "LuauComponentFields.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include "CollisionBox.hpp"
#include "ComponentRegistry.hpp"
#include "GameScriptingConstants.hpp"
#include "Gun.hpp"
#include "Health.hpp"
#include "LuauComponentConstants.hpp"
#include "LuauScriptError.hpp"
#include "LuauSteppedBody.hpp"
#include "Position.hpp"
#include "ScoreValue.hpp"
#include "SteadyMotion.hpp"
#include "lua.h"

namespace rtype::game {

namespace {

/** @returns The component of @p body a write of @p key changes; raises an
 * error when the body has none. */
template <typename Component>
Component& writtenComponentOf(lua_State* state, const LuauSteppedBody& body,
                              std::string_view key) {
  auto* const found = body.components->get<Component>(body.entity);
  if (found == nullptr) {
    raiseAtScriptLine(state, fieldText(key, ABSENT_COMPONENT_ERROR));
  }
  return *found;
}

/** @brief Pushes what @p push makes of the component of @p body, or nil when
 * the body has none. */
template <typename Component, typename Push>
void pushIfPresent(lua_State* state, const LuauSteppedBody& body, Push push) {
  const auto* const found = body.components->get<Component>(body.entity);
  if (found == nullptr) {
    lua_pushnil(state);
    return;
  }
  push(state, *found);
}

/** @brief Pushes a table of the two @p values, under the @p keys of the same
 * place. */
void pushPair(lua_State* state, std::array<const char*, 2> keys,
              std::array<double, 2> values) {
  lua_createtable(state, 0, 2);
  for (std::size_t index = 0; index < keys.size(); ++index) {
    lua_pushnumber(state, values.at(index));
    lua_setfield(state, -2, keys.at(index));
  }
}

/** @returns The number written, nothing when the value is no number. */
std::optional<double> writtenNumber(lua_State* state) {
  if (lua_type(state, WRITTEN_VALUE_ARGUMENT) != LUA_TNUMBER) {
    return std::nullopt;
  }
  return lua_tonumber(state, WRITTEN_VALUE_ARGUMENT);
}

/** @returns The whole number written, from @p lowest to @p highest; nothing
 * for any other value. */
std::optional<double> writtenWholeNumber(lua_State* state, double lowest,
                                         double highest) {
  const std::optional<double> number = writtenNumber(state);
  if (!number.has_value() || std::floor(*number) != *number ||
      *number < lowest || *number > highest) {
    return std::nullopt;
  }
  return number;
}

/** @returns The two numbers of the table written, under @p keys, as floats;
 * nothing when the value is no table or one of them is no finite float. */
std::optional<std::array<float, 2>> writtenFinitePair(
    lua_State* state, std::array<const char*, 2> keys) {
  if (!lua_istable(state, WRITTEN_VALUE_ARGUMENT)) {
    return std::nullopt;
  }
  std::array<float, 2> pair{};
  bool finite = true;
  for (std::size_t index = 0; index < keys.size(); ++index) {
    const bool isNumber = lua_rawgetfield(state, WRITTEN_VALUE_ARGUMENT,
                                          keys.at(index)) == LUA_TNUMBER;
    pair.at(index) = static_cast<float>(lua_tonumber(state, -1));
    finite = finite && isNumber && std::isfinite(pair.at(index));
    lua_pop(state, 1);
  }
  if (!finite) {
    return std::nullopt;
  }
  return pair;
}

void writePosition(lua_State* state, const LuauSteppedBody& body) {
  auto& position =
      writtenComponentOf<Position>(state, body, POSITION_COMPONENT_KEY);
  const std::optional<std::array<float, 2>> point =
      writtenFinitePair(state, {POINT_X_KEY, POINT_Y_KEY});
  if (!point.has_value()) {
    raiseAtScriptLine(state, POSITION_REFUSAL);
  }
  position = Position{.x = (*point)[0], .y = (*point)[1]};
}

void writeHealth(lua_State* state, const LuauSteppedBody& body) {
  auto& health = writtenComponentOf<Health>(state, body, HEALTH_COMPONENT_KEY);
  const std::optional<double> points = writtenWholeNumber(
      state, LOWEST_WRITTEN_HEALTH, static_cast<double>(health.full));
  if (!points.has_value()) {
    raiseAtScriptLine(state, HEALTH_REFUSAL);
  }
  health.points = static_cast<int>(*points);
}

void writeCollisionBox(lua_State* state, const LuauSteppedBody& body) {
  auto& box = writtenComponentOf<CollisionBox>(state, body,
                                               COLLISION_BOX_COMPONENT_KEY);
  const std::optional<std::array<float, 2>> size =
      writtenFinitePair(state, {BOX_WIDTH_KEY, BOX_HEIGHT_KEY});
  if (!size.has_value() || (*size)[0] <= 0.0F || (*size)[1] <= 0.0F) {
    raiseAtScriptLine(state, COLLISION_BOX_REFUSAL);
  }
  box.width = (*size)[0];
  box.height = (*size)[1];
}

void writeTopSpeed(lua_State* state, const LuauSteppedBody& body) {
  auto& motion =
      writtenComponentOf<SteadyMotion>(state, body, TOP_SPEED_COMPONENT_KEY);
  const std::optional<double> number = writtenNumber(state);
  const auto speed = static_cast<float>(number.value_or(0.0));
  if (!std::isfinite(speed) || speed <= 0.0F) {
    raiseAtScriptLine(state, TOP_SPEED_REFUSAL);
  }
  motion.topSpeed = speed;
}

void writeScore(lua_State* state, const LuauSteppedBody& body) {
  auto& score =
      writtenComponentOf<ScoreValue>(state, body, SCORE_COMPONENT_KEY);
  const std::optional<double> points =
      writtenWholeNumber(state, LOWEST_WRITTEN_SCORE, HIGHEST_WRITTEN_SCORE);
  if (!points.has_value()) {
    raiseAtScriptLine(state, SCORE_REFUSAL);
  }
  score.points = static_cast<std::uint32_t>(*points);
}

void writeGunInterval(lua_State* state, const LuauSteppedBody& body) {
  auto& gun = writtenComponentOf<Gun>(state, body, GUN_INTERVAL_COMPONENT_KEY);
  const std::optional<double> ticks = writtenWholeNumber(
      state, LOWEST_WRITTEN_GUN_INTERVAL, HIGHEST_WRITTEN_GUN_INTERVAL);
  if (!ticks.has_value()) {
    raiseAtScriptLine(state, GUN_INTERVAL_REFUSAL);
  }
  gun.intervalTicks = static_cast<std::size_t>(*ticks);
}

/** @brief Pushes the component @p key of @p body that a script reads as one
 * number: its health, full health, top speed, score or gun interval. */
void pushNumberField(lua_State* state, const LuauSteppedBody& body,
                     std::string_view key) {
  if (key == HEALTH_COMPONENT_KEY) {
    pushIfPresent<Health>(state, body, [](lua_State* target, Health health) {
      lua_pushnumber(target, health.points);
    });
  } else if (key == FULL_HEALTH_COMPONENT_KEY) {
    pushIfPresent<Health>(state, body, [](lua_State* target, Health health) {
      lua_pushnumber(target, health.full);
    });
  } else if (key == TOP_SPEED_COMPONENT_KEY) {
    pushIfPresent<SteadyMotion>(state, body,
                                [](lua_State* target, SteadyMotion motion) {
                                  lua_pushnumber(target, motion.topSpeed);
                                });
  } else if (key == SCORE_COMPONENT_KEY) {
    pushIfPresent<ScoreValue>(state, body,
                              [](lua_State* target, ScoreValue score) {
                                lua_pushnumber(target, score.points);
                              });
  } else {
    pushIfPresent<Gun>(state, body, [](lua_State* target, const Gun& gun) {
      lua_pushnumber(target, static_cast<double>(gun.intervalTicks));
    });
  }
}

}  // namespace

std::string fieldText(std::string_view key, std::string_view what) {
  std::string text{HANDLE_FIELD_PREFIX};
  text += key;
  text += what;
  return text;
}

bool isComponentKey(std::string_view key) {
  return std::ranges::find(COMPONENT_KEYS, key) != COMPONENT_KEYS.end();
}

void pushComponentField(lua_State* state, const LuauSteppedBody& body,
                        std::string_view key) {
  if (key == POSITION_COMPONENT_KEY) {
    pushIfPresent<Position>(state, body, [](lua_State* target, Position point) {
      pushPair(target, {POINT_X_KEY, POINT_Y_KEY}, {point.x, point.y});
    });
  } else if (key == COLLISION_BOX_COMPONENT_KEY) {
    pushIfPresent<CollisionBox>(
        state, body, [](lua_State* target, CollisionBox box) {
          pushPair(target, {BOX_WIDTH_KEY, BOX_HEIGHT_KEY},
                   {box.width, box.height});
        });
  } else {
    pushNumberField(state, body, key);
  }
}

void writeComponentField(lua_State* state, const LuauSteppedBody& body,
                         std::string_view key) {
  if (key == POSITION_COMPONENT_KEY) {
    writePosition(state, body);
  } else if (key == HEALTH_COMPONENT_KEY) {
    writeHealth(state, body);
  } else if (key == COLLISION_BOX_COMPONENT_KEY) {
    writeCollisionBox(state, body);
  } else if (key == TOP_SPEED_COMPONENT_KEY) {
    writeTopSpeed(state, body);
  } else if (key == SCORE_COMPONENT_KEY) {
    writeScore(state, body);
  } else if (key == GUN_INTERVAL_COMPONENT_KEY) {
    writeGunInterval(state, body);
  } else {
    raiseAtScriptLine(state, fieldText(key, READ_ONLY_FIELD_ERROR));
  }
}

}  // namespace rtype::game
