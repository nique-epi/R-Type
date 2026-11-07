#pragma once

#include <SFML/Graphics/Color.hpp>
#include <array>
#include <chrono>
#include <numbers>
#include "BackgroundColorConstants.hpp"
#include "InterfaceColorConstants.hpp"
#include "LandWave.hpp"

namespace rtype::client {

/**
 * @brief The planet of the map as a halftone, as the mockup draws it: a cell
 * every HALFTONE_PITCH pixels holds a dot whose size and blue follow the light
 * of that point of the sphere; past the edge the dots thin out at random over
 * PLANET_FADE_IN pixels inside it and PLANET_FADE_OUT outside, so the planet
 * has no outline. A dot thinner than HALFTONE_SMALL_KEEP keeps 1 pixel.
 */
inline constexpr int HALFTONE_PITCH = 3;
inline constexpr int PLANET_FADE_IN = 6;
inline constexpr double HALFTONE_SMALL_KEEP = 0.7;

/** @brief The light: from the top right, wrapped around the night side, with
 * a rim on the lit edge. The sun is (SUN_ACROSS, SUN_DOWN, SUN_TOWARDS_VIEWER)
 * made a unit vector. */
inline constexpr double SUN_ACROSS = 0.5;
inline constexpr double SUN_DOWN = -0.72;
inline constexpr double SUN_TOWARDS_VIEWER = 0.48;
inline constexpr double LIGHT_WRAP = 0.35;
inline constexpr double LIGHT_AMBIENT = 0.06;
inline constexpr double LIGHT_RIM = 0.35;
inline constexpr double RIM_SHARPNESS = 5.0;
inline constexpr double RIM_NIGHT_MARGIN = 0.1;
inline constexpr double LIGHT_OF_THE_SEA = 0.82;
inline constexpr double LIGHT_OF_THE_LAND = 0.28;

/**
 * @brief The lands: LAND_LEVEL plus four waves that wrap around the longitude,
 * so a turn of the longitude turns the planet with no seam; the last one is a
 * cosine, a sine a quarter turn ahead.
 */
inline constexpr double LAND_LEVEL = 0.5;
inline constexpr double QUARTER_TURN = std::numbers::pi / 2.0;
inline constexpr std::array<LandWave, 4> LAND_WAVES{
    LandWave{.longitude = 3.0,
             .latitude = 2.0,
             .phase = 0.0,
             .weight = 0.22,
             .latitudeBend = 2.0},
    LandWave{.longitude = 5.0,
             .latitude = -3.0,
             .phase = 1.0,
             .weight = 0.16,
             .latitudeBend = 0.0},
    LandWave{.longitude = 9.0,
             .latitude = 7.0,
             .phase = 2.0,
             .weight = 0.09,
             .latitudeBend = 0.0},
    LandWave{.longitude = 2.0,
             .latitude = 5.0,
             .phase = QUARTER_TURN,
             .weight = 0.06,
             .latitudeBend = 0.0}};

/** @brief The grains of the ring: two waves around it, (RING_GRAIN_TURNS_A *
 * angle + RING_GRAIN_RADII_A * radius) times (RING_GRAIN_TURNS_B * angle -
 * RING_GRAIN_RADII_B * radius). */
inline constexpr double RING_GRAIN_TURNS_A = 9.0;
inline constexpr double RING_GRAIN_RADII_A = 23.0;
inline constexpr double RING_GRAIN_TURNS_B = 4.0;
inline constexpr double RING_GRAIN_RADII_B = 11.0;

/** @brief Where the noise of the ring is read, away from the noise of the
 * sphere at the same cell. */
inline constexpr int RING_NOISE_ROW_SHIFT = 7;
inline constexpr int RING_NOISE_COLUMN_SHIFT = 3;

/** @brief One whole turn of the planet or its ring, in radians. */
inline constexpr double FULL_TURN_RADIANS = 2.0 * std::numbers::pi;
/** @brief The levels of a dot, thresholded with BAYER_MATRIX: none, then a
 * blue and a size per level. */
inline constexpr int DOT_LEVEL_COUNT = 4;
inline constexpr std::array<sf::Color, 4> DOT_COLORS{DEEP_BLUE, BLUE, PALE_BLUE,
                                                     HUD_CYAN};
inline constexpr std::array<int, 4> DOT_SIZES{1, 1, 2, 2};

/**
 * @brief The ring: a disc of the equator from RING_INNER_RADII to
 * RING_OUTER_RADII radii, two bands and a dark gap between RING_GAP_START and
 * RING_GAP_END radii, its grains drifting round in RING_TURN. In the shadow of
 * the planet a grain keeps RING_SHADOW_LIGHT of its light; in front of the
 * planet it covers a cell only as often as RING_FRONT_COVER of its density.
 */
inline constexpr double RING_INNER_RADII = 1.4;
inline constexpr double RING_GAP_START = 1.68;
inline constexpr double RING_GAP_END = 1.75;
inline constexpr double RING_GAP_DENSITY = 0.08;
inline constexpr double RING_INNER_BAND_TOP = 0.95;
inline constexpr double RING_INNER_BAND_FALL = 0.35;
inline constexpr double RING_OUTER_BAND_TOP = 0.75;
inline constexpr double RING_OUTER_BAND_FALL = 0.3;
inline constexpr double RING_BAND_RIPPLE = 0.15;
inline constexpr double RING_BAND_RIPPLES_PER_RADIUS = 60.0;
inline constexpr double RING_SHADOW_LIGHT = 0.35;
inline constexpr double RING_FRONT_COVER = 0.9;
inline constexpr double RING_GRAIN_BASE = 0.7;
inline constexpr double RING_GRAIN_SPREAD = 0.3;
inline constexpr double RING_TONE_SPREAD = 0.2;
inline constexpr double RING_DARKEST_TONE = 0.12;
inline constexpr double RING_BLUE_TONE = 0.24;
inline constexpr double RING_PALE_TONE = 0.45;
inline constexpr double RING_AXIS_EDGE_ON = 0.01;

/** @brief The noise that thins the dots out, read cell by cell, the same at
 * every frame. */
inline constexpr double NOISE_ACROSS = 12.9898;
inline constexpr double NOISE_DOWN = 78.233;
inline constexpr double NOISE_SCALE = 43758.5453;

/** @brief One turn of the planet and one of the grains of its ring, and the
 * frames per second the turn is drawn at. */
inline constexpr std::chrono::milliseconds PLANET_TURN{30'000};
inline constexpr std::chrono::milliseconds RING_TURN{20'000};
inline constexpr int PLANET_FRAMES_PER_SECOND = 24;

}  // namespace rtype::client
