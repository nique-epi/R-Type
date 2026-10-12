#pragma once

#include <SFML/System/Vector2.hpp>
#include <array>
#include <cstddef>
#include <string_view>
#include "BackgroundColors.hpp"
#include "SkyEventKind.hpp"
#include "TailGradient.hpp"

namespace rtype::client {

/**
 * @brief How many times faster than real time sky events animate.
 *
 * The chosen design gives events speeds and lifetimes made for a sky three
 * times narrower than the background, and runs them three times faster: they
 * cross the sky in the same time, and flares and shooting stars live a third
 * as long. Event speeds and lifetimes below are in background pixels and
 * seconds of this clock.
 */
constexpr float SKY_EVENT_CLOCK_RATE = 3.0F;

/** @brief Most sky events shown at once. */
constexpr std::size_t MAXIMUM_SKY_EVENTS = 4;

/**
 * @brief Real time before the first event, and bounds of the real time drawn
 * between the start of an event and the start of the next, in seconds. When
 * the sky is full, a due event starts as soon as another one ends.
 */
constexpr float FIRST_SKY_EVENT_DELAY = 0.6F;
constexpr float SHORTEST_SKY_EVENT_GAP = 0.8F;
constexpr float LONGEST_SKY_EVENT_GAP = 2.6F;

constexpr std::size_t SKY_EVENT_KIND_COUNT =
    static_cast<std::size_t>(SkyEventKind::Comet) + 1;

/**
 * @brief Relative chance of each kind, in the order of SkyEventKind: shooting
 * stars are the most frequent, comets the rarest.
 */
constexpr std::array<double, SKY_EVENT_KIND_COUNT> SKY_EVENT_WEIGHTS{4.0, 1.5,
                                                                     1.5, 1.0};

/**
 * @brief Shooting star: speed, angle below the horizontal towards the left in
 * radians, start in the right part and top part of the sky as shares of its
 * size, life, and trail length, growth and shrink.
 */
constexpr float SHOOTING_STAR_SLOWEST_SPEED = 100.0F;
constexpr float SHOOTING_STAR_FASTEST_SPEED = 170.0F;
constexpr float SHOOTING_STAR_SHALLOWEST_ANGLE = 0.3F;
constexpr float SHOOTING_STAR_STEEPEST_ANGLE = 0.7F;
constexpr float SHOOTING_STAR_LEFTMOST_START = 0.35F;
constexpr float SHOOTING_STAR_LOWEST_START = 0.45F;
constexpr float SHOOTING_STAR_SHORTEST_LIFE = 0.4F;
constexpr float SHOOTING_STAR_LONGEST_LIFE = 1.0F;
constexpr float SHOOTING_STAR_LONGEST_TRAIL = 12.0F;
constexpr float SHOOTING_STAR_TRAIL_GROWTH = 60.0F;
constexpr float SHOOTING_STAR_TRAIL_SHRINK = 50.0F;
constexpr TailGradient SHOOTING_STAR_GRADIENT{
    GradientStop{.end = 0.15F, .color = STAR_WHITE},
    GradientStop{.end = 0.4F, .color = PALE_BLUE},
    GradientStop{.end = 0.7F, .color = BLUE},
    GradientStop{.end = 1.0F, .color = DEEP_BLUE},
};

/**
 * @brief Flare: a star that flares up in a cross then fades out. Distance
 * from the edges, duration, longest arm in pixels, arm length from which the
 * diagonals light up, and glow above which the core turns white.
 */
constexpr float FLARE_EDGE_MARGIN = 4.0F;
constexpr float FLARE_SHORTEST_DURATION = 2.0F;
constexpr float FLARE_LONGEST_DURATION = 3.0F;
constexpr float FLARE_LONGEST_ARM = 3.4F;
constexpr int FLARE_SHORTEST_ARM_WITH_DIAGONALS = 2;
constexpr float FLARE_WHITE_CORE_GLOW = 0.6F;

/**
 * @brief Asteroid: speed towards the left, largest vertical drift, spin in
 * frames per second, and how far beyond the edges it starts and ends.
 */
constexpr float ASTEROID_SLOWEST_SPEED = 14.0F;
constexpr float ASTEROID_FASTEST_SPEED = 36.0F;
constexpr float ASTEROID_LARGEST_DRIFT = 3.0F;
constexpr float ASTEROID_SLOWEST_SPIN = 2.0F;
constexpr float ASTEROID_FASTEST_SPIN = 6.0F;
constexpr float ASTEROID_OFF_SKY_MARGIN = 6.0F;

/**
 * @brief Frames of a spinning asteroid, ASTEROID_SIZE pixels square: 'd' is
 * in shadow, 'l' in light, '.' is empty.
 */
constexpr std::size_t ASTEROID_SIZE = 4;
using AsteroidFrame = std::array<std::string_view, ASTEROID_SIZE>;
constexpr char ASTEROID_EMPTY = '.';
constexpr char ASTEROID_LIGHT = 'l';
constexpr std::array ASTEROID_FRAMES{
    AsteroidFrame{".dl.", "dlld", "dddl", ".dd."},
    AsteroidFrame{".ld.", "ddll", "lddd", ".dd."},
    AsteroidFrame{".dd.", "lddd", "lldd", ".ld."},
    AsteroidFrame{".dd.", "dddl", "dlld", ".ld."},
};

/**
 * @brief Comet: speed towards the left, largest vertical drift, distance from
 * the top and bottom edges where it enters, and how far beyond the edges it
 * starts and ends.
 */
constexpr float COMET_SLOWEST_SPEED = 16.0F;
constexpr float COMET_FASTEST_SPEED = 26.0F;
constexpr float COMET_LARGEST_DRIFT = 2.5F;
constexpr float COMET_EDGE_MARGIN = 10.0F;
constexpr float COMET_ENTRY_MARGIN = 4.0F;
constexpr float COMET_OFF_SKY_MARGIN = 32.0F;

/**
 * @brief Comet tail, COMET_TAIL_LENGTH pixels behind the head.
 *
 * Beyond its first COMET_SOLID_TAIL pixels, one pixel in COMET_GAP_PERIOD is
 * left out, and the gaps run along the tail COMET_GAP_STEPS_PER_SECOND times
 * per second. Beyond COMET_FLICKER_START of its length, each pixel is hidden
 * with a chance of COMET_FLICKER_CHANCE at every frame. Before
 * COMET_THICK_TAIL, every other pixel is three pixels thick.
 */
constexpr std::size_t COMET_TAIL_LENGTH = 28;
constexpr std::size_t COMET_SOLID_TAIL = 5;
constexpr std::size_t COMET_GAP_PERIOD = 3;
constexpr float COMET_GAP_STEPS_PER_SECOND = 8.0F;
constexpr float COMET_FLICKER_START = 0.55F;
constexpr float COMET_FLICKER_CHANCE = 0.45F;
constexpr std::size_t COMET_THICK_TAIL = 9;
constexpr std::size_t COMET_THICK_TAIL_STRIDE = 2;
constexpr TailGradient COMET_GRADIENT{
    GradientStop{.end = 0.18F, .color = COMET_CORE_COLOR},
    GradientStop{.end = 0.4F, .color = COMET_GLOW_COLOR},
    GradientStop{.end = 0.7F, .color = BLUE},
    GradientStop{.end = 1.0F, .color = DEEP_BLUE},
};

/** @brief The four pixels of a comet head, from its top-left one. */
constexpr std::array COMET_HEAD_OFFSETS{sf::Vector2i(0, 0), sf::Vector2i(1, 0),
                                        sf::Vector2i(0, 1), sf::Vector2i(1, 1)};

}  // namespace rtype::client
