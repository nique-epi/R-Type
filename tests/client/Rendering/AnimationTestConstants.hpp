#pragma once

#include <array>
#include <chrono>
#include "Animation.hpp"
#include "SpriteFrame.hpp"
#include "TimeConstants.hpp"

/** @brief Three frames of an 8 × 8 sprite, side by side in a sheet. */
constexpr rtype::game::SpriteFrame FIRST{
    .left = 0, .top = 0, .width = 8, .height = 8};
constexpr rtype::game::SpriteFrame SECOND{
    .left = 10, .top = 0, .width = 8, .height = 8};
constexpr rtype::game::SpriteFrame THIRD{
    .left = 20, .top = 0, .width = 8, .height = 8};
constexpr std::array THREE_FRAMES{FIRST, SECOND, THIRD};

/** @brief How long each of the three frames shows, and all three together. */
constexpr rtype::engine::Duration FRAME_TIME = std::chrono::milliseconds(100);
constexpr rtype::engine::Duration THREE_FRAMES_LONG =
    std::chrono::milliseconds(300);

/** @brief The three frames in a loop, the three frames played once, and an
 * animation without frames. */
constexpr rtype::client::Animation LOOP{
    .frames = THREE_FRAMES,
    .frameDuration = FRAME_TIME,
    .playback = rtype::client::Animation::Playback::Loop};
constexpr rtype::client::Animation ONCE{
    .frames = THREE_FRAMES,
    .frameDuration = FRAME_TIME,
    .playback = rtype::client::Animation::Playback::Once};
constexpr rtype::client::Animation WITHOUT_FRAMES{
    .frames = {},
    .frameDuration = FRAME_TIME,
    .playback = rtype::client::Animation::Playback::Loop};
