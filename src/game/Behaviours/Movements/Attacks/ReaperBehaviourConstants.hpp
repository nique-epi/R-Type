#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include "ActionSettingConstants.hpp"
#include "BehaviourConstants.hpp"
#include "BehaviourSetting.hpp"
#include "WholeNumberRange.hpp"

namespace rtype::game {

/**
 * @brief The settings of the reaper, in logical units and milliseconds: how
 * far to the right of the nearest ship it stops (`reach`, between the centers),
 * how near that point is near enough to strike (`align`, on each axis), then
 * the rest and the phases of its swing (see ActionSettingConstants.hpp).
 */
constexpr std::string_view REACH_SETTING_NAME = "reach";
constexpr std::string_view ALIGN_SETTING_NAME = "align";
constexpr engine::WholeNumberRange ALIGN_RANGE{.lowest = 1, .highest = 512};
constexpr std::array REAPER_SETTINGS{
    BehaviourSetting{.name = REACH_SETTING_NAME, .range = PLAYFIELD_SPAN_RANGE},
    BehaviourSetting{.name = ALIGN_SETTING_NAME, .range = ALIGN_RANGE},
    BehaviourSetting{.name = REST_SETTING_NAME, .range = REST_RANGE},
    BehaviourSetting{.name = SIGNAL_SETTING_NAME, .range = PHASE_RANGE},
    BehaviourSetting{.name = STRIKE_SETTING_NAME, .range = PHASE_RANGE},
    BehaviourSetting{.name = RECOVER_SETTING_NAME, .range = RECOVER_RANGE}};

/** @brief Where each setting is among the values of BehaviourView::settings.
 * The strike and the recover follow the signal (see timingsAt()). */
constexpr std::size_t REACH_SETTING = 0;
constexpr std::size_t ALIGN_SETTING = 1;
constexpr std::size_t REAPER_REST_SETTING = 2;
constexpr std::size_t REAPER_SIGNAL_SETTING = 3;

/** @brief The attack number of the swing of the scythe, which the sheet of the
 * reaper animates and times its hurt boxes with. */
constexpr std::uint8_t SCYTHE_ATTACK = 0;

}  // namespace rtype::game
