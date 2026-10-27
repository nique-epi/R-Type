#pragma once

#include <chrono>
#include <cstdint>
#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief The closed book drops from above, its easeOutBack reaching where
 * it lies at half its time (then sinking a little and coming back): that is
 * when it lands, its dust puffs and its impact sounds. */
constexpr engine::Duration DROP_START = std::chrono::milliseconds(100);
constexpr engine::Duration DROP_TIME = std::chrono::milliseconds(600);
constexpr std::int64_t EASE_OUT_BACK_LANDING_DIVISOR = 2;
constexpr engine::Duration LANDING =
    DROP_START + (DROP_TIME / EASE_OUT_BACK_LANDING_DIVISOR);
constexpr engine::Duration DUST_TIME = std::chrono::milliseconds(400);

/** @brief The taunt is typed on the label of the closed book, held so it is
 * read, then the cover turns while the label falls off. */
constexpr engine::Duration TAUNT_START = std::chrono::milliseconds(850);
constexpr engine::Duration TAUNT_LETTER_TIME = std::chrono::milliseconds(30);
constexpr engine::Duration TAUNT_HOLD = std::chrono::milliseconds(900);
constexpr engine::Duration OPEN_TIME = std::chrono::milliseconds(700);
constexpr engine::Duration LABEL_FALL_TIME = std::chrono::milliseconds(350);

/** @brief The left page, from the moment the book lies open. */
constexpr engine::Duration MISSION_LOG_DELAY = std::chrono::milliseconds(50);
constexpr engine::Duration MISSION_LOG_LETTER_TIME =
    std::chrono::milliseconds(30);
constexpr engine::Duration STAMP_DELAY = std::chrono::milliseconds(300);
constexpr engine::Duration STAMP_SLAM_FRAME_TIME =
    std::chrono::milliseconds(50);
constexpr engine::Duration SHAKE_TIME = std::chrono::milliseconds(150);
constexpr engine::Duration SHAKE_STEP = std::chrono::milliseconds(16);
constexpr engine::Duration DOWN_DELAY = std::chrono::milliseconds(550);
constexpr engine::Duration DOWN_LETTER_TIME = std::chrono::milliseconds(50);
constexpr engine::Duration PHOTOS_DELAY = std::chrono::milliseconds(1350);
constexpr engine::Duration PHOTO_STEP = std::chrono::milliseconds(150);
constexpr engine::Duration PHOTO_DROP_TIME = std::chrono::milliseconds(150);
constexpr engine::Duration TEAM_DELAY = std::chrono::milliseconds(1450);
constexpr engine::Duration TEAM_COUNT_TIME = std::chrono::milliseconds(1000);

/** @brief The right page, once ALL PILOTS DOWN is written: its heading, then
 * a row per pilot, ROW_STEP apart, each written over ROW_TIME. */
constexpr engine::Duration RANKING_DELAY = std::chrono::milliseconds(1500);
constexpr engine::Duration RANKING_LETTER_TIME = std::chrono::milliseconds(40);
constexpr engine::Duration RULE_DRAW_TIME = std::chrono::milliseconds(250);
constexpr engine::Duration ROWS_DELAY = std::chrono::milliseconds(2300);
constexpr engine::Duration ROW_STEP = std::chrono::milliseconds(400);
constexpr engine::Duration ROW_TIME = std::chrono::milliseconds(800);
constexpr engine::Duration DIGIT_SLAM_TIME = std::chrono::milliseconds(50);
constexpr engine::Duration NAME_DELAY = std::chrono::milliseconds(50);
constexpr engine::Duration NAME_LETTER_TIME = std::chrono::milliseconds(30);
constexpr engine::Duration LEADER_DELAY = std::chrono::milliseconds(300);
constexpr engine::Duration LEADER_TIME = std::chrono::milliseconds(250);
constexpr engine::Duration ROW_COUNT_TIME = std::chrono::milliseconds(450);
constexpr engine::Duration MEDAL_TIME = std::chrono::milliseconds(400);

/** @brief The call to play again, typed after the last row, then blinking:
 * shown BLINK_STEPS - 1 steps, hidden one. */
constexpr engine::Duration PROMPT_DELAY = std::chrono::milliseconds(200);
constexpr engine::Duration PROMPT_LETTER_TIME = std::chrono::milliseconds(20);
constexpr engine::Duration BLINK_STEP = std::chrono::milliseconds(300);
constexpr std::int64_t BLINK_STEPS = 3;

/** @brief After a choice, the book closes, then slides off to the right
 * (PLAY AGAIN) or falls off (QUIT). */
constexpr engine::Duration EXIT_CLOSE_TIME = std::chrono::milliseconds(450);
constexpr engine::Duration EXIT_MOVE_TIME = std::chrono::milliseconds(300);
constexpr engine::Duration EXIT_TIME = EXIT_CLOSE_TIME + EXIT_MOVE_TIME;

}  // namespace rtype::client
