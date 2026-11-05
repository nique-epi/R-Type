#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include "ActionSettingConstants.hpp"
#include "BehaviourConstants.hpp"
#include "BehaviourSetting.hpp"
#include "PlayfieldConstants.hpp"
#include "WeaveBehaviourConstants.hpp"
#include "WholeNumberRange.hpp"

namespace rtype::game {

/**
 * @brief The settings of the bydomancer, in logical units and milliseconds:
 * the column it stops at (`stand_x`), how far it leaps once wounded (`jump`,
 * at most half the area of play), how far it bobs above and below its row
 * (`bob`) and how long a bob takes (`bob_period_ms`), the rest and the phases
 * of its orb (see ActionSettingConstants.hpp), how near a ship makes it cast
 * its nova (`nova_reach`, between the centers) and how long the nova lasts
 * (`nova_ms`).
 */
constexpr std::string_view STAND_X_SETTING_NAME = "stand_x";
constexpr std::string_view JUMP_SETTING_NAME = "jump";
constexpr std::string_view BOB_SETTING_NAME = "bob";
constexpr std::string_view BOB_PERIOD_SETTING_NAME = "bob_period_ms";
constexpr std::string_view NOVA_REACH_SETTING_NAME = "nova_reach";
constexpr std::string_view NOVA_SETTING_NAME = "nova_ms";
constexpr engine::WholeNumberRange JUMP_RANGE{
    .lowest = 1, .highest = static_cast<std::int64_t>(PLAY_AREA_HEIGHT / 2.0F)};
constexpr std::array BYDOMANCER_SETTINGS{
    BehaviourSetting{.name = STAND_X_SETTING_NAME,
                     .range = PLAYFIELD_SPAN_RANGE},
    BehaviourSetting{.name = JUMP_SETTING_NAME, .range = JUMP_RANGE},
    BehaviourSetting{.name = BOB_SETTING_NAME, .range = AMPLITUDE_RANGE},
    BehaviourSetting{.name = BOB_PERIOD_SETTING_NAME, .range = PERIOD_RANGE},
    BehaviourSetting{.name = REST_SETTING_NAME, .range = REST_RANGE},
    BehaviourSetting{.name = SIGNAL_SETTING_NAME, .range = PHASE_RANGE},
    BehaviourSetting{.name = STRIKE_SETTING_NAME, .range = PHASE_RANGE},
    BehaviourSetting{.name = RECOVER_SETTING_NAME, .range = RECOVER_RANGE},
    BehaviourSetting{.name = NOVA_REACH_SETTING_NAME,
                     .range = PLAYFIELD_SPAN_RANGE},
    BehaviourSetting{.name = NOVA_SETTING_NAME, .range = PHASE_RANGE}};

/** @brief Where each setting is among the values of BehaviourView::settings.
 * The strike and the recover of the orb follow its signal (see
 * timingsAt()). */
constexpr std::size_t STAND_X_SETTING = 0;
constexpr std::size_t JUMP_SETTING = 1;
constexpr std::size_t BOB_SETTING = 2;
constexpr std::size_t BOB_PERIOD_SETTING = 3;
constexpr std::size_t BYDOMANCER_REST_SETTING = 4;
constexpr std::size_t BYDOMANCER_SIGNAL_SETTING = 5;
constexpr std::size_t NOVA_REACH_SETTING = 8;
constexpr std::size_t NOVA_SETTING = 9;

/** @brief The attack numbers of the orb and of the nova, which the sheet of
 * the bydomancer animates and times its hurt boxes with. */
constexpr std::uint8_t ORB_ATTACK = 0;
constexpr std::uint8_t NOVA_ATTACK = 1;

}  // namespace rtype::game
