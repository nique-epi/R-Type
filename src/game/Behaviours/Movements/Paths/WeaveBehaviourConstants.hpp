#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include "BehaviourSetting.hpp"
#include "SpawningConstants.hpp"
#include "WholeNumberRange.hpp"

namespace rtype::game {

/**
 * @brief The settings of the weave: how far it goes above and below its row,
 * in logical units, and how long it takes to go down, up and back to its row,
 * in milliseconds.
 *
 * The amplitude stays within ENEMY_PATTERN_ROOM, the room every row keeps above
 * and below it, so a body on a weave stays in the area of play; the reader of
 * the sheets refuses a wider one.
 */
constexpr std::string_view AMPLITUDE_SETTING_NAME = "amplitude";
constexpr engine::WholeNumberRange AMPLITUDE_RANGE{
    .lowest = 0, .highest = static_cast<std::int64_t>(ENEMY_PATTERN_ROOM)};
constexpr std::string_view PERIOD_SETTING_NAME = "period_ms";
constexpr engine::WholeNumberRange PERIOD_RANGE{.lowest = 1, .highest = 60000};
constexpr std::array WEAVE_SETTINGS{
    BehaviourSetting{.name = AMPLITUDE_SETTING_NAME, .range = AMPLITUDE_RANGE},
    BehaviourSetting{.name = PERIOD_SETTING_NAME, .range = PERIOD_RANGE}};

/** @brief Where each setting is among the values of BehaviourView::settings.
 */
constexpr std::size_t AMPLITUDE_SETTING = 0;
constexpr std::size_t PERIOD_SETTING = 1;

}  // namespace rtype::game
