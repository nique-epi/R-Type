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
 * @brief The settings of the drillback, in logical units and milliseconds:
 * how fast it drifts to the left when it is not charging (`drift_speed`, at
 * most its top speed, which is the speed of its charge), the column left of
 * which it may charge (`enter_x`), then the rest and the phases of its charge
 * (see ActionSettingConstants.hpp): the signal is its shaking, the strike its
 * charge, the recover its stun.
 */
constexpr std::string_view DRIFT_SPEED_SETTING_NAME = "drift_speed";
constexpr std::string_view ENTER_X_SETTING_NAME = "enter_x";
constexpr engine::WholeNumberRange DRIFT_SPEED_RANGE{.lowest = 0,
                                                     .highest = 4096};
constexpr std::array DRILLBACK_SETTINGS{
    BehaviourSetting{.name = DRIFT_SPEED_SETTING_NAME,
                     .range = DRIFT_SPEED_RANGE},
    BehaviourSetting{.name = ENTER_X_SETTING_NAME,
                     .range = PLAYFIELD_SPAN_RANGE},
    BehaviourSetting{.name = REST_SETTING_NAME, .range = REST_RANGE},
    BehaviourSetting{.name = SIGNAL_SETTING_NAME, .range = PHASE_RANGE},
    BehaviourSetting{.name = STRIKE_SETTING_NAME, .range = PHASE_RANGE},
    BehaviourSetting{.name = RECOVER_SETTING_NAME, .range = RECOVER_RANGE}};

/** @brief Where each setting is among the values of BehaviourView::settings.
 * The strike and the recover follow the signal (see timingsAt()). */
constexpr std::size_t DRIFT_SPEED_SETTING = 0;
constexpr std::size_t ENTER_X_SETTING = 1;
constexpr std::size_t DRILLBACK_REST_SETTING = 2;
constexpr std::size_t DRILLBACK_SIGNAL_SETTING = 3;

/** @brief The attack number of the charge, which the sheet of the drillback
 * animates and names in its fragile table. */
constexpr std::uint8_t CHARGE_ATTACK = 0;

}  // namespace rtype::game
