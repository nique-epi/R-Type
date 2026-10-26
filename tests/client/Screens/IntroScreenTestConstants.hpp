#pragma once

#include <chrono>
#include <cstdint>
#include "TimeConstants.hpp"

namespace rtype::client::testing {

/** @brief Seed of the intros of the tests. */
constexpr std::uint32_t INTRO_SCREEN_SEED = 7;

/** @brief The intro gives way to the title after ten beats of 60/109 s,
 * 5.505 s: the tests play a little less and a little more. */
constexpr engine::Duration JUST_BEFORE_THE_TITLE =
    std::chrono::milliseconds(5400);
constexpr engine::Duration JUST_AFTER_THE_TITLE =
    std::chrono::milliseconds(5600);

/** @brief The music 3 s in, after the squadron entered (2.75 s); 4.9 s in,
 * in the full warp of the boost; 4.45 s in, just after the release
 * (4.40 s). */
constexpr engine::Duration MUSIC_AFTER_THE_ENTRY = std::chrono::seconds(3);
constexpr engine::Duration MUSIC_IN_THE_WARP = std::chrono::milliseconds(4900);
constexpr engine::Duration MUSIC_AFTER_THE_RELEASE =
    std::chrono::milliseconds(4450);

/** @brief Frames played after the release, enough for the noise of the
 * shake to leave zero on one of them. */
constexpr int FRAMES_AFTER_THE_RELEASE = 10;

/** @brief The stars warp twelve times their speed, and come back to it. */
constexpr float WARP = 12.0F;
constexpr float OWN_SPEED = 1.0F;

/** @brief A press 1.23 s into the music, between its second and third beats:
 * the logo then lands 0.3 s after the third beat, at 3 × 60/109 + 0.3 = 1.951 s
 * of the music, 0.721 s after the press; the tests play a little less and a
 * little more. */
constexpr engine::Duration PRESS_IN_THE_MUSIC = std::chrono::milliseconds(1230);
constexpr engine::Duration BEFORE_THE_SKIPPED_LANDING =
    std::chrono::milliseconds(700);
constexpr engine::Duration AFTER_THE_SKIPPED_LANDING =
    std::chrono::milliseconds(740);

}  // namespace rtype::client::testing
