#pragma once

#include <SFML/System/Vector2.hpp>
#include <array>
#include <chrono>
#include "AudioConstants.hpp"
#include "EffectConstants.hpp"
#include "IntroFreeze.hpp"

namespace rtype::client {

/** @brief One beat of the theme: the intro cuts on its beats. Every time of
 * the intro is in seconds since the first beat of the theme. */
constexpr float INTRO_BEAT = THEME_BEAT;

/** @brief The intro opens on INTRO_BLACK_TIME seconds of black before its
 * time 0, so the player sees it start; it starts the music at
 * INTRO_MUSIC_START, THEME_FIRST_BEAT before time 0, so the first beat of
 * the theme lands on time 0. */
constexpr float INTRO_BLACK_TIME = 1.0F;
constexpr float INTRO_MUSIC_START =
    -std::chrono::duration<float>(THEME_FIRST_BEAT).count();

/**
 * @brief The beats of the intro, in seconds of the music: the Bydo in the
 * dark until the first beat, one close-up per pilot from the first, the wide
 * shot of the swarm at INTRO_SWARM_CUT, its first kill, the charge, the
 * release of the beams, the slam of the logo, then the title screen.
 */
constexpr float INTRO_SWARM_CUT = 5.0F * INTRO_BEAT;
constexpr float INTRO_FIRST_KILL = 5.75F * INTRO_BEAT;
constexpr float INTRO_CHARGE = 7.0F * INTRO_BEAT;
constexpr float INTRO_RELEASE = 8.0F * INTRO_BEAT;
constexpr float INTRO_SLAM = 9.0F * INTRO_BEAT;
constexpr float INTRO_TITLE = 10.0F * INTRO_BEAT;

/** @brief Two moments the mockup sets on beats of the scene itself: the swap
 * of the pink and green ships, and the arrival of the reinforcements; after
 * the first freeze, each comes INTRO_FIRST_IMPACT_FREEZE after its beat of
 * the music. */
constexpr float INTRO_SWAP = 6.0F * INTRO_BEAT;
constexpr float INTRO_REINFORCEMENTS = 6.5F * INTRO_BEAT;

/**
 * @brief The freezes take the holds of the game: the hold of a kill at the
 * first impact, and the hold of a big catch when the ring bursts,
 * INTRO_BURST_DELAY after the release.
 */
constexpr float INTRO_FIRST_IMPACT_FREEZE =
    std::chrono::duration<float>(KILL_HOLD).count();
constexpr float INTRO_BURST_FREEZE =
    std::chrono::duration<float>(BIG_CATCH_HOLD).count();
constexpr float INTRO_BURST_DELAY = 0.03F;
constexpr std::array INTRO_FREEZES{
    IntroFreeze{.at = INTRO_FIRST_KILL, .length = INTRO_FIRST_IMPACT_FREEZE},
    IntroFreeze{.at = INTRO_RELEASE + INTRO_BURST_DELAY,
                .length = INTRO_BURST_FREEZE}};
constexpr float INTRO_FREEZES_LENGTH =
    INTRO_FIRST_IMPACT_FREEZE + INTRO_BURST_FREEZE;

/**
 * @brief Moments of the scene after the freezes: the charge and the release,
 * behind their beats by the first freeze; the boost, INTRO_BOOST_DELAY after
 * the release; the slam, the end of the full warp INTRO_WARP_HOLD after it,
 * and the title, behind their beats by both freezes.
 */
constexpr float INTRO_SCENE_CHARGE = INTRO_CHARGE - INTRO_FIRST_IMPACT_FREEZE;
constexpr float INTRO_SCENE_RELEASE = INTRO_RELEASE - INTRO_FIRST_IMPACT_FREEZE;
constexpr float INTRO_BOOST_DELAY = 0.1F;
constexpr float INTRO_SCENE_BOOST = INTRO_SCENE_RELEASE + INTRO_BOOST_DELAY;
constexpr float INTRO_SCENE_SLAM = INTRO_SLAM - INTRO_FREEZES_LENGTH;
constexpr float INTRO_WARP_HOLD = 0.2F;
constexpr float INTRO_SCENE_WARP_END = INTRO_SCENE_SLAM + INTRO_WARP_HOLD;
constexpr float INTRO_SCENE_TITLE = INTRO_TITLE - INTRO_FREEZES_LENGTH;

/** @brief The logo slams in INTRO_SLAM_TIME seconds of the music from
 * INTRO_SLAM, and lands then. */
constexpr float INTRO_SLAM_TIME = 0.3F;

/**
 * @brief Speeds of the stars, as ISky takes them: INTRO_COLD_OPEN_SPEED on
 * the Bydo in the dark, INTRO_CLOSEUP_SPEED behind the close-ups, from
 * INTRO_SWARM_FIRST_SPEED down to INTRO_SWARM_SPEED in INTRO_SWARM_SLOWDOWN
 * behind the swarm, down to INTRO_CHARGE_SPEED during the charge, up to
 * WARP_TOP_SPEED_FACTOR in INTRO_WARP_RISE at the boost, then back to their
 * own speed exactly when the title opens.
 */
constexpr float INTRO_COLD_OPEN_SPEED = 0.6F;
constexpr float INTRO_CLOSEUP_SPEED = 8.0F;
constexpr float INTRO_SWARM_FIRST_SPEED = 6.0F;
constexpr float INTRO_SWARM_SPEED = 3.0F;
constexpr float INTRO_SWARM_SLOWDOWN = 0.5F;
constexpr float INTRO_CHARGE_SPEED = 0.3F;
constexpr float INTRO_CHARGE_SLOWDOWN = 0.8F * INTRO_BEAT;
constexpr float INTRO_WARP_RISE = 0.12F;
constexpr float INTRO_OWN_SPEED = 1.0F;

/**
 * @brief The shots: the cold open and each close-up zoom INTRO_CLOSEUP_ZOOM
 * times, on INTRO_COLD_OPEN_FOCUS, then on INTRO_CLOSEUP_FOCUS moved down by
 * the offset of the pilot, in the order of the player colors; the wide shot
 * shows the whole world.
 */
constexpr int INTRO_CLOSEUP_ZOOM = 3;
constexpr sf::Vector2f INTRO_COLD_OPEN_FOCUS(330.0F, 135.0F);
constexpr sf::Vector2f INTRO_CLOSEUP_FOCUS(240.0F, 135.0F);
constexpr std::array INTRO_CLOSEUP_OFFSETS{-20.0F, 20.0F, -10.0F, 10.0F};

/** @brief The debris lie in the plane of the ships, much closer than the
 * nearest stars: they drift left INTRO_DEBRIS_SPEED pixels a second of the
 * way the sky goes, counted in steps of INTRO_SKY_STEP from the boost. */
constexpr float INTRO_DEBRIS_SPEED = 90.0F;
constexpr float INTRO_SKY_STEP = 1.0F / 60.0F;

/** @brief A flame, a charge or a tremble flickers
 * INTRO_FLICKER_STEPS_PER_SECOND times a second. */
constexpr float INTRO_FLICKER_STEPS_PER_SECOND = 30.0F;

}  // namespace rtype::client
