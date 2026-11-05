#pragma once

#include <array>
#include <cstddef>
#include <string_view>
#include "BehaviourSetting.hpp"
#include "WholeNumberRange.hpp"

namespace rtype::game {

/**
 * @brief The one setting of the straight behaviour: which way its body goes
 * along its row, -1 to the left (towards the ships: a Drifter, a missile of
 * an enemy), 1 to the right (a missile of a player), 0 for a body that stays.
 */
constexpr std::string_view HEADING_SETTING_NAME = "heading";
constexpr engine::WholeNumberRange HEADING_RANGE{.lowest = -1, .highest = 1};
constexpr std::array STRAIGHT_SETTINGS{
    BehaviourSetting{.name = HEADING_SETTING_NAME, .range = HEADING_RANGE}};

/** @brief Where the heading is among the values of BehaviourView::settings.
 */
constexpr std::size_t HEADING_SETTING = 0;

}  // namespace rtype::game
