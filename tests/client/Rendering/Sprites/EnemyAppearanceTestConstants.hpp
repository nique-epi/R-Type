#pragma once

#include <array>
#include <chrono>
#include "ActionKey.hpp"
#include "Animation.hpp"
#include "EnemyAppearance.hpp"
#include "EnemyPhase.hpp"
#include "SpriteFrame.hpp"
#include "TimeConstants.hpp"

namespace rtype::client::testing {

/** @brief The frames of an idle loop and of two attack animations. */
constexpr game::SpriteFrame IDLE_FRAME{
    .left = 0, .top = 0, .width = 24, .height = 24};
constexpr game::SpriteFrame WIDE_FRAME{
    .left = 0, .top = 26, .width = 40, .height = 24};
constexpr std::array IDLE_FRAMES{IDLE_FRAME};
constexpr std::array WIDE_FRAMES{WIDE_FRAME};

/** @brief How long a frame shows. */
constexpr engine::Duration TEST_FRAME_TIME = std::chrono::milliseconds(100);

/** @brief The attacks of the appearance under test. */
constexpr std::uint8_t SCYTHE_ATTACK = 1;
constexpr std::uint8_t UNKNOWN_ATTACK = 9;

/** @brief The anchor of the wide frames: the body of 24 pixels is the right
 * part of a frame of 40, so the frame is drawn 8 pixels to the left. */
constexpr sf::Vector2i WIDE_ANCHOR(-8, 0);

constexpr Animation TEST_IDLE{.frames = IDLE_FRAMES,
                              .frameDuration = TEST_FRAME_TIME,
                              .playback = Animation::Playback::Loop};

constexpr std::array TEST_ACTIONS{
    ActionLook{.attack = SCYTHE_ATTACK,
               .phase = game::EnemyPhase::Signal,
               .animation = {.frames = WIDE_FRAMES,
                             .frameDuration = TEST_FRAME_TIME,
                             .playback = Animation::Playback::Hold,
                             .anchor = WIDE_ANCHOR}},
    ActionLook{.attack = SCYTHE_ATTACK,
               .phase = game::EnemyPhase::Recover,
               .animation = {.frames = WIDE_FRAMES,
                             .frameDuration = TEST_FRAME_TIME,
                             .playback = Animation::Playback::Once,
                             .anchor = WIDE_ANCHOR}}};

constexpr EnemyAppearance TEST_APPEARANCE{
    .sheetId = "sheet", .idle = TEST_IDLE, .actions = TEST_ACTIONS};

}  // namespace rtype::client::testing
