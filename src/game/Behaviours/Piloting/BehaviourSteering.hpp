#pragma once

#include <cstddef>
#include <cstdint>
#include "BehaviourView.hpp"
#include "TimeConstants.hpp"

namespace rtype::game {

/**
 * @returns The vertical share that moves the body of @p view to @p height
 * during this tick, 0 when it cannot move during it (no top speed, or a step
 * of no time). A behaviour that follows a path computed from the age of its
 * body writes it, so the path does not depend on how the time is cut into
 * steps.
 */
[[nodiscard]] float verticalShareTowards(float height,
                                         const BehaviourView& view);

/**
 * @returns The value of the setting at @p index of the settings the behaviour
 * of @p view declares.
 * @throws MissingBehaviourSettingException when @p view holds no value there:
 * a body made without the settings of its sheet, which the reader of the
 * sheets never gives.
 */
[[nodiscard]] std::int64_t settingAt(const BehaviourView& view,
                                     std::size_t index);

/**
 * @returns The setting at @p index of the behaviour of @p view, a number of
 * milliseconds, as a duration.
 * @throws MissingBehaviourSettingException as settingAt() does.
 */
[[nodiscard]] engine::Duration millisecondsSettingAt(const BehaviourView& view,
                                                     std::size_t index);

}  // namespace rtype::game
