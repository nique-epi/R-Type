#pragma once

#include <SFML/System/Vector2.hpp>
#include <array>
#include <cstddef>
#include "EffectConstants.hpp"
#include "EntitySpriteConstants.hpp"
#include "IntroSparkSpread.hpp"
#include "IntroTimelineConstants.hpp"
#include "RenderingConstants.hpp"
#include "SquadronConstants.hpp"

namespace rtype::client {

/** @brief Half of each sprite of the intro, which places it by its
 * center. */
constexpr sf::Vector2f INTRO_SHIP_HALF =
    sf::Vector2f(static_cast<float>(PLAYER_SHIP_FRAME.width),
                 static_cast<float>(PLAYER_SHIP_FRAME.height)) *
    HALF;
constexpr sf::Vector2f INTRO_MOB_HALF =
    sf::Vector2f(static_cast<float>(BYDO_FRAME.width),
                 static_cast<float>(BYDO_FRAME.height)) *
    HALF;
constexpr sf::Vector2f INTRO_PLAYER_MISSILE_HALF =
    sf::Vector2f(static_cast<float>(PLAYER_MISSILE_FRAME.width),
                 static_cast<float>(PLAYER_MISSILE_FRAME.height)) *
    HALF;
constexpr sf::Vector2f INTRO_ENEMY_MISSILE_HALF =
    sf::Vector2f(static_cast<float>(ENEMY_MISSILE_FRAME.width),
                 static_cast<float>(ENEMY_MISSILE_FRAME.height)) *
    HALF;
constexpr sf::Vector2f INTRO_IMPACT_HALF =
    sf::Vector2f(static_cast<float>(IMPACT_SIZE),
                 static_cast<float>(IMPACT_SIZE)) *
    HALF;
constexpr sf::Vector2f INTRO_BYDO_BURST_HALF =
    sf::Vector2f(static_cast<float>(BYDO_EXPLOSION_SIZE),
                 static_cast<float>(BYDO_EXPLOSION_SIZE)) *
    HALF;
constexpr sf::Vector2f INTRO_FIREBALL_HALF =
    sf::Vector2f(static_cast<float>(SHIP_EXPLOSION_WIDTH),
                 static_cast<float>(SHIP_EXPLOSION_HEIGHT)) *
    HALF;

/** @brief Close-ups: the ship of a pilot sweeps INTRO_CLOSEUP_SWEEP pixels
 * across its beat, slowest in its middle, and wobbles one pixel,
 * INTRO_CLOSEUP_WOBBLE radians per beat. */
constexpr float INTRO_CLOSEUP_SWEEP = 760.0F;
constexpr float INTRO_CLOSEUP_WOBBLE = 6.0F;

/**
 * @brief The squadron of the wide shot. Each pilot, in the order of the
 * player colors, has its home; they enter from INTRO_ENTRY_X one after the
 * other, INTRO_ENTRY_STAGGER apart, each in INTRO_ENTRY_FLIGHT, and are ready
 * to fire INTRO_READY_DELAY after the cut, at INTRO_SQUADRON_READY: no missile
 * of the squadron leaves before.
 */
constexpr std::array INTRO_HOMES{
    sf::Vector2f(180.0F, 127.0F), sf::Vector2f(150.0F, 103.0F),
    sf::Vector2f(150.0F, 151.0F), sf::Vector2f(120.0F, 127.0F)};
constexpr float INTRO_ENTRY_X = -70.0F;
constexpr float INTRO_ENTRY_STAGGER = 0.04F;
constexpr float INTRO_ENTRY_FLIGHT = 0.3F;
constexpr float INTRO_READY_DELAY = 0.2F;
constexpr float INTRO_SQUADRON_READY = INTRO_SWARM_CUT + INTRO_READY_DELAY;

/** @brief The pink and green ships swap rows from INTRO_SWAP, in
 * INTRO_SWAP_TIME, bulging INTRO_SWAP_BULGE pixels forward and back. */
constexpr std::size_t INTRO_PINK_PILOT = 1;
constexpr std::size_t INTRO_GREEN_PILOT = 2;
constexpr float INTRO_SWAP_TIME = 0.35F;
constexpr float INTRO_SWAP_BULGE = 14.0F;

/** @brief The squadron sways INTRO_SWAY pixels every INTRO_SWAY_PERIOD, the
 * sway fading in INTRO_SWAY_FADE at the charge; each ship bobs one pixel
 * every INTRO_BOB_PERIOD, INTRO_BOB_PHASE_STEP radians after the one before
 * it. */
constexpr float INTRO_SWAY = 5.0F;
constexpr float INTRO_SWAY_PERIOD = 1.1F;
constexpr float INTRO_SWAY_FADE = 0.2F;
constexpr float INTRO_BOB_PERIOD = 1.6F;
constexpr float INTRO_BOB_PHASE_STEP = 1.3F;

/** @brief The charge pulls the squadron back INTRO_PULL_BACK pixels in
 * INTRO_PULL_BACK_TIME; it trembles during the last INTRO_TREMBLE_TIME before
 * the release, recoils INTRO_RECOIL pixels at it, losing INTRO_RECOIL_DECAY
 * a second; from the boost, each ship lands on its place in the squadron of
 * the title in INTRO_LANDING_TIME, INTRO_LANDING_STAGGER after the one before
 * it, all of them before the title opens. */
constexpr float INTRO_PULL_BACK = 8.0F;
constexpr float INTRO_PULL_BACK_TIME = 0.3F;
constexpr float INTRO_TREMBLE_TIME = 0.15F;
constexpr float INTRO_RECOIL = 6.0F;
constexpr float INTRO_RECOIL_DECAY = 12.0F;
constexpr float INTRO_LANDING_STAGGER = 0.03F;
constexpr float INTRO_LANDING_TIME = 0.7F;

/** @brief Length of the flame of the ships, from 0 to 1: in the close-ups, at
 * the entry, settled behind the swarm in INTRO_EXHAUST_SETTLE, during the
 * charge, and at the boost, before it dies out as the ship lands. */
constexpr float INTRO_CLOSEUP_EXHAUST = 0.8F;
constexpr float INTRO_ENTRY_EXHAUST = 0.9F;
constexpr float INTRO_SWARM_EXHAUST = 0.45F;
constexpr float INTRO_EXHAUST_SETTLE = 0.4F;
constexpr float INTRO_CHARGE_EXHAUST = 0.15F;
constexpr float INTRO_BOOST_EXHAUST = 1.0F;

/** @brief The ghosts of a ship fade in INTRO_ENTRY_SMEAR_FADE after its
 * entry starts; they come back at the boost and catch up with the ship as it
 * lands. */
constexpr float INTRO_ENTRY_SMEAR_FADE = 0.35F;

/** @brief The nose of a ship, from its top-left corner: one pixel past its
 * sprite, one row under its muzzle. */
constexpr sf::Vector2f INTRO_NOSE(static_cast<float>(PLAYER_SHIP_FRAME.width) +
                                      1.0F,
                                  SQUADRON_MUZZLE.y + 1.0F);

/**
 * @brief The Bydos of the close-ups: the Bydo in the dark drifts left
 * INTRO_COLD_OPEN_DRIFT pixels a second from INTRO_COLD_OPEN_FOCUS and fires
 * at INTRO_COLD_OPEN_SHOT_START; a Bydo crosses the close-up of
 * INTRO_CROSSED_PILOT and a bonus Bydo that of INTRO_BONUS_CROSSED_PILOT,
 * from INTRO_CROSSER_START_X at INTRO_CROSSER_SPEED pixels a second, each on
 * its row; INTRO_PREY_PILOT shoots the prey, which drifts left
 * INTRO_PREY_SPEED pixels a second from INTRO_PREY_START, at INTRO_PREY_HIT.
 */
constexpr std::size_t INTRO_OPENING_MOBS = 4;
constexpr float INTRO_COLD_OPEN_DRIFT = 10.0F;
constexpr sf::Vector2f INTRO_COLD_OPEN_SHOT_FROM(318.0F, 135.0F);
constexpr sf::Vector2f INTRO_COLD_OPEN_SHOT_TO(244.0F, 138.0F);
constexpr float INTRO_COLD_OPEN_SHOT_START = 0.3F;
constexpr std::size_t INTRO_CROSSED_PILOT = 1;
constexpr std::size_t INTRO_BONUS_CROSSED_PILOT = 3;
constexpr float INTRO_CROSSER_START_X = 340.0F;
constexpr float INTRO_CROSSER_SPEED = 420.0F;
constexpr float INTRO_CROSSER_ROW = 132.0F;
constexpr float INTRO_BONUS_CROSSER_ROW = 160.0F;
constexpr std::size_t INTRO_PREY_PILOT = 2;
constexpr sf::Vector2f INTRO_PREY_START(300.0F, 127.0F);
constexpr float INTRO_PREY_SPEED = 30.0F;
constexpr float INTRO_PREY_APPEARS = 3.0F * INTRO_BEAT;
constexpr float INTRO_PREY_HIT = INTRO_PREY_APPEARS + 0.36F;

/**
 * @brief Missiles: a missile of a player flies INTRO_PLAYER_MISSILE_SPEED
 * pixels a second, aimed in INTRO_AIM_PASSES passes from a first guess of
 * INTRO_AIM_FIRST_GUESS; the ship that fires at a Bydo of the helix is the
 * one closest in height INTRO_AIM_LOOKBACK before the hit. The missile of
 * kill i swerves INTRO_KILL_CURVE + i INTRO_KILL_CURVE_STEP pixels, on each
 * side in turn, wobbling INTRO_SWERVE_WOBBLE pixels INTRO_SWERVE_WAVES
 * radians along. A missile of an enemy flies INTRO_ENEMY_MISSILE_SPEED.
 */
constexpr float INTRO_PLAYER_MISSILE_SPEED = 420.0F;
constexpr float INTRO_ENEMY_MISSILE_SPEED = 170.0F;
constexpr float INTRO_AIM_FIRST_GUESS = 0.3F;
constexpr int INTRO_AIM_PASSES = 4;
constexpr float INTRO_AIM_LOOKBACK = 0.1F;
constexpr float INTRO_KILL_CURVE = 16.0F;
constexpr float INTRO_KILL_CURVE_STEP = 2.0F;
constexpr float INTRO_SWERVE_WOBBLE = 3.0F;
constexpr float INTRO_SWERVE_WAVES = 18.0F;

/**
 * @brief The helix: INTRO_HELIX_SIZE Bydos on INTRO_HELIX_STRANDS strands,
 * entering INTRO_HELIX_LEAD before the cut, INTRO_HELIX_STAGGER apart, from
 * INTRO_HELIX_START_X at INTRO_HELIX_SPEED pixels a second, waving
 * INTRO_HELIX_AMPLITUDE pixels around INTRO_HELIX_MIDDLE_Y,
 * INTRO_HELIX_WAVE radians a second; the last two are bonus Bydos. Bydo i
 * is hit INTRO_KILL_GAPS[i] after the first kill, faster and faster; it
 * flashes INTRO_HIT_FLASH_LEAD before and dies INTRO_DEATH_DELAY after.
 */
constexpr std::size_t INTRO_HELIX_SIZE = 16;
constexpr std::size_t INTRO_HELIX_STRANDS = 2;
constexpr float INTRO_HELIX_LEAD = 0.3F;
constexpr float INTRO_HELIX_STAGGER = 0.07F;
constexpr float INTRO_HELIX_START_X = 500.0F;
constexpr float INTRO_HELIX_SPEED = 230.0F;
constexpr float INTRO_HELIX_MIDDLE_Y = 135.0F;
constexpr float INTRO_HELIX_AMPLITUDE = 46.0F;
constexpr float INTRO_HELIX_WAVE = 5.2F;
constexpr std::size_t INTRO_HELIX_FIRST_BONUS = 14;
constexpr std::array INTRO_KILL_GAPS{
    0.0F,  0.05F,  0.095F, 0.135F, 0.17F,  0.205F, 0.235F, 0.265F,
    0.29F, 0.315F, 0.335F, 0.355F, 0.375F, 0.39F,  0.405F, 0.42F};
constexpr float INTRO_HIT_FLASH_LEAD = 0.017F;
constexpr float INTRO_DEATH_DELAY = 0.02F;

/** @brief At the swap, the Bydos INTRO_SHOOTING_BYDOS of the helix fire at
 * INTRO_HELIX_SHOT_TARGET_X, past the left edge, that many pixels up and
 * down. */
constexpr std::array<std::size_t, 2> INTRO_SHOOTING_BYDOS{9, 12};
constexpr std::array INTRO_HELIX_SHOT_DROPS{-30.0F, 30.0F};
constexpr float INTRO_HELIX_SHOT_TARGET_X = -20.0F;

/**
 * @brief The ring: INTRO_RING_SIZE newcomers around INTRO_RING_CENTER,
 * turning INTRO_RING_SPIN radians a second, plus INTRO_RING_CHARGE_SPIN times
 * the square of the time since the charge; its radius shrinks from
 * INTRO_RING_RADIUS to INTRO_RING_CHARGED_RADIUS over a beat of charge,
 * flattened to INTRO_RING_FLATTEN. Newcomer i arrives INTRO_NEWCOMER_STAGGER
 * after the one before it, and glides in INTRO_NEWCOMER_GLIDE from
 * INTRO_NEWCOMER_REACH + i INTRO_NEWCOMER_REACH_STEP pixels right of its
 * place, INTRO_NEWCOMER_SWOOP + i INTRO_NEWCOMER_SWOOP_STEP pixels above or
 * below it, on each side in turn; the last two are bonus Bydos. Newcomer i
 * is hit INTRO_RING_HIT_DELAY + i INTRO_RING_HIT_STAGGER after the release.
 */
constexpr std::size_t INTRO_RING_SIZE = 8;
constexpr sf::Vector2f INTRO_RING_CENTER(352.0F, 135.0F);
constexpr float INTRO_RING_SPIN = 2.0F;
constexpr float INTRO_RING_CHARGE_SPIN = 6.0F;
constexpr float INTRO_RING_RADIUS = 34.0F;
constexpr float INTRO_RING_CHARGED_RADIUS = 20.0F;
constexpr float INTRO_RING_FLATTEN = 0.8F;
constexpr float INTRO_NEWCOMER_STAGGER = 0.035F;
constexpr float INTRO_NEWCOMER_GLIDE = 0.45F;
constexpr float INTRO_NEWCOMER_REACH = 170.0F;
constexpr float INTRO_NEWCOMER_REACH_STEP = 6.0F;
constexpr float INTRO_NEWCOMER_SWOOP = 30.0F;
constexpr float INTRO_NEWCOMER_SWOOP_STEP = 4.0F;
constexpr std::size_t INTRO_RING_FIRST_BONUS = 6;
constexpr float INTRO_RING_HIT_DELAY = 0.02F;
constexpr float INTRO_RING_HIT_STAGGER = 0.008F;

/** @brief Newcomers and curved missiles swoop on INTRO_SIDES sides in
 * turn. */
constexpr std::size_t INTRO_SIDES = 2;

/**
 * @brief Bursts: a small one shows one burst of a Bydo, INTRO_SMALL_SPARK_COUNT
 * sparks and some drops (INTRO_HELIX_DROPS for a Bydo of the helix,
 * INTRO_RING_DROPS for a newcomer, none for the prey); the big one of the ring
 * shows INTRO_BIG_BLASTS explosions scattered over INTRO_BIG_BLAST_SPREAD,
 * INTRO_BIG_BLAST_STAGGER apart, every INTRO_BIG_BLAST_PERIOD-th a burst of
 * a Bydo and the others fireballs, INTRO_BIG_SPARK_COUNT sparks and
 * INTRO_BIG_DROP_COUNT drops. Nothing of a burst shows after
 * INTRO_BURST_LIFE.
 */
constexpr std::size_t INTRO_SMALL_SPARK_COUNT = 8;
constexpr std::size_t INTRO_HELIX_DROPS = 6;
constexpr std::size_t INTRO_RING_DROPS = 5;
constexpr std::size_t INTRO_BIG_BLASTS = 11;
constexpr sf::Vector2f INTRO_BIG_BLAST_SPREAD(72.0F, 60.0F);
constexpr float INTRO_BIG_BLAST_STAGGER = 0.045F;
constexpr std::size_t INTRO_BIG_BLAST_PERIOD = 3;
constexpr std::size_t INTRO_BIG_SPARK_COUNT = 28;
constexpr std::size_t INTRO_BIG_DROP_COUNT = 24;
constexpr float INTRO_BURST_LIFE = 0.9F;

/** @brief How the sparks and the drops of each burst are drawn, and their
 * two sizes. */
constexpr IntroSparkSpread INTRO_SMALL_SPARKS{.slowest = 40.0F,
                                              .fastest = 110.0F,
                                              .shortestLife = 0.25F,
                                              .longestLife = 0.5F,
                                              .latestStart = 0.05F,
                                              .largeChance = 0.3F};
constexpr IntroSparkSpread INTRO_BIG_SPARKS{.slowest = 60.0F,
                                            .fastest = 180.0F,
                                            .shortestLife = 0.25F,
                                            .longestLife = 0.5F,
                                            .latestStart = 0.3F,
                                            .largeChance = 0.3F};
constexpr IntroSparkSpread INTRO_SMALL_DROPS{.slowest = 30.0F,
                                             .fastest = 90.0F,
                                             .shortestLife = 0.35F,
                                             .longestLife = 0.65F,
                                             .latestStart = 0.04F,
                                             .largeChance = 1.0F};
constexpr IntroSparkSpread INTRO_BIG_DROPS{.slowest = 30.0F,
                                           .fastest = 140.0F,
                                           .shortestLife = 0.35F,
                                           .longestLife = 0.65F,
                                           .latestStart = 0.04F,
                                           .largeChance = 1.0F};
constexpr int INTRO_SMALL_SPARK = 1;
constexpr int INTRO_LARGE_SPARK = 2;

/** @brief Rings: each Bydo of the helix leaves one of INTRO_KILL_RING_RADIUS
 * for INTRO_KILL_RING_LIFE; the ring bursts into one of INTRO_BIG_RING_RADIUS
 * for INTRO_BIG_RING_LIFE. */
constexpr float INTRO_KILL_RING_RADIUS = 14.0F;
constexpr float INTRO_KILL_RING_LIFE = 0.28F;
constexpr float INTRO_BIG_RING_RADIUS = 46.0F;
constexpr float INTRO_BIG_RING_LIFE = 0.45F;

/**
 * @brief The charge: INTRO_CHARGE_GRAINS grains of the light shade of the
 * pilot, started over INTRO_GRAIN_SPREAD of the charge, each flying
 * INTRO_GRAIN_FLIGHT from INTRO_GRAIN_REACH pixels to the nose along a curve
 * of power INTRO_GRAIN_CURVE, turning INTRO_GRAIN_TURN radians, white past
 * INTRO_GRAIN_WHITE_AFTER of its flight; every INTRO_LARGE_GRAIN_PERIOD-th is
 * INTRO_LARGE_GRAIN_WIDTH pixels wide; the hash of each is seeded with the
 * pilot times INTRO_GRAIN_SEED_STEP plus its number. The ball grows from
 * INTRO_BALL_RADIUS by INTRO_BALL_GROWTH pixels and stays
 * INTRO_CHARGE_AFTERGLOW past the release.
 */
constexpr std::size_t INTRO_CHARGE_GRAINS = 16;
constexpr float INTRO_GRAIN_SPREAD = 0.8F;
constexpr float INTRO_GRAIN_FLIGHT = 0.3F;
constexpr float INTRO_GRAIN_TURN = 2.4F;
constexpr float INTRO_GRAIN_REACH = 34.0F;
constexpr float INTRO_GRAIN_CURVE = 1.5F;
constexpr float INTRO_GRAIN_WHITE_AFTER = 0.7F;
constexpr std::size_t INTRO_LARGE_GRAIN_PERIOD = 3;
constexpr int INTRO_LARGE_GRAIN_WIDTH = 2;
constexpr std::size_t INTRO_GRAIN_SEED_STEP = 31;
constexpr float INTRO_BALL_RADIUS = 1.0F;
constexpr float INTRO_BALL_GROWTH = 4.0F;
constexpr float INTRO_CHARGE_AFTERGLOW = 0.04F;

/**
 * @brief The beams last INTRO_BEAM_TIME from the release: each reaches
 * INTRO_BEAM_OVERSHOOT pixels past the right edge within INTRO_BEAM_REACH_TIME
 * of its life, fades from INTRO_BEAM_FADE_START of it, and is
 * INTRO_BEAM_HALF pixels thick on each side of its middle row, waving one
 * pixel INTRO_BEAM_WAVE radians a second.
 */
constexpr float INTRO_BEAM_TIME = 0.22F;
constexpr float INTRO_BEAM_OVERSHOOT = 40.0F;
constexpr float INTRO_BEAM_REACH_TIME = 0.25F;
constexpr float INTRO_BEAM_FADE_START = 0.6F;
constexpr float INTRO_BEAM_HALF = 3.0F;
constexpr float INTRO_BEAM_WAVE = 70.0F;

/**
 * @brief The traumas the cues add to the shake of the camera (CameraShake:
 * SHAKE_MAXIMUM_OFFSET pixels times the trauma squared), each chosen so that
 * alone it gives the offset the mockup shook by: 2 pixels at the prey, from 1
 * to 2.2 pixels over the kills of the cascade, 4 pixels at the release and 3
 * pixels INTRO_AFTERSHOCK_DELAY later.
 */
constexpr float INTRO_PREY_TRAUMA = 0.58F;
constexpr float INTRO_FIRST_KILL_TRAUMA = 0.41F;
constexpr float INTRO_LAST_KILL_TRAUMA = 0.61F;
constexpr float INTRO_RELEASE_TRAUMA = 0.82F;
constexpr float INTRO_AFTERSHOCK_TRAUMA = 0.71F;
constexpr float INTRO_AFTERSHOCK_DELAY = 0.12F;

/** @brief Each close-up starts with its flyby INTRO_FLYBY_DELAY after its
 * beat. */
constexpr float INTRO_FLYBY_DELAY = 0.05F;

}  // namespace rtype::client
