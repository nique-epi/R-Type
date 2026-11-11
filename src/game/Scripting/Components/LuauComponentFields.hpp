#pragma once

#include <string>
#include <string_view>
#include "LuauSteppedBody.hpp"

struct lua_State;

namespace rtype::game {

/** @returns `e.` @p key followed by @p what, the text of an error about the
 * field @p key of a handle. */
[[nodiscard]] std::string fieldText(std::string_view key,
                                    std::string_view what);

/** @returns Whether @p key names one of the components of COMPONENT_KEYS. */
[[nodiscard]] bool isComponentKey(std::string_view key);

/**
 * @brief Pushes on @p state the component @p key of @p body, as a script
 * reads it: `position` `{x, y}`, `health` and `full_health` the points of its
 * Health, `collision_box` `{width, height}`, `top_speed` the speed of its
 * SteadyMotion, `score` the points of its ScoreValue, `gun_interval` the ticks
 * between two shots of its Gun; nil when the body has no such component.
 *
 * @p key is one of COMPONENT_KEYS, and the components of @p body are not
 * null.
 */
void pushComponentField(lua_State* state, const LuauSteppedBody& body,
                        std::string_view key);

/**
 * @brief Writes the value at WRITTEN_VALUE_ARGUMENT of @p state into the
 * component @p key of @p body, read as pushComponentField() pushes it. The
 * offset of a collision box stays.
 *
 * Raises an error of the script, and writes nothing, when @p key is
 * `full_health`, when the body has no such component, or when the value is
 * not of its kind or out of its bounds: a position of two finite numbers; a
 * health a whole number from LOWEST_WRITTEN_HEALTH to the full health of the
 * body; a collision box and a top speed positive finite numbers; a score
 * and a gun interval whole numbers within the bounds of
 * LuauComponentConstants.hpp.
 *
 * @p key is one of COMPONENT_KEYS, and the components of @p body are not
 * null.
 */
void writeComponentField(lua_State* state, const LuauSteppedBody& body,
                         std::string_view key);

}  // namespace rtype::game
