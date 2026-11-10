#pragma once

#include <chrono>
#include <cstdint>
#include <string_view>
#include "FrameAnchor.hpp"
#include "Position.hpp"
#include "SpriteFrame.hpp"
#include "TimeConstants.hpp"

namespace rtype::client::testing {

/** @brief The body the effects are played on, where it is, and an id the
 * client does not have. */
constexpr std::uint32_t EFFECT_BODY_ID = 12;
constexpr std::uint32_t UNKNOWN_BODY_ID = 13;
constexpr game::Position EFFECT_BODY_POSITION{.x = 300.0F, .y = 400.0F};

/** @brief The sheet of the body, and its one effect: spark, a first frame of
 * 16 by 16 pixels at (0, 30), its version without flash in the row under it,
 * at (0, 50), hanging 8 pixels above the body. */
constexpr std::string_view EFFECT_BODY_SHEET = "sprites/mobs/reaper.png";
constexpr std::string_view SPARK = "spark";
constexpr std::string_view UNKNOWN_EFFECT = "smoke";
constexpr game::SpriteFrame SPARK_FIRST_FRAME{
    .left = 0, .top = 30, .width = 16, .height = 16};
constexpr game::SpriteFrame SPARK_FIRST_FRAME_WITHOUT_FLASH{
    .left = 0, .top = 50, .width = 16, .height = 16};
constexpr FrameAnchor SPARK_ANCHOR{.x = 0, .y = -8};
constexpr engine::Duration SPARK_FRAME_TIME = std::chrono::milliseconds(60);

}  // namespace rtype::client::testing
