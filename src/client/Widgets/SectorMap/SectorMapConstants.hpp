#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <array>
#include <cstdint>

namespace rtype::client {

/**
 * @brief The map of the servers, on the 480 x 270 grid of the interface: its
 * frame, the planet of the player cut in half by the left edge of the frame,
 * and where the chosen server stands once zoomed in.
 */
inline constexpr sf::IntRect SECTOR_MAP_AREA{{24, 52}, {432, 168}};
inline constexpr int SECTOR_PLANET_RADIUS = 60;
inline constexpr sf::Vector2i SECTOR_PLANET_CENTER{24, 136};
inline constexpr sf::Vector2i CHOSEN_STAR_ZOOMED_IN{300, 126};
inline constexpr double ZOOMED_OUT_FACTOR = 1.0;
inline constexpr double ZOOMED_IN_FACTOR = 2.0;

/**
 * @brief The ring of the planet, a disc in its equator seen at an angle: it
 * reaches RING_OUTER_RADII radii of the planet, and the axis of the planet
 * leans in "/", so the ring reads "\". The axis is (RING_AXIS_ACROSS *
 * RING_AXIS_LEAN, -RING_AXIS_DOWN * RING_AXIS_LEAN, RING_AXIS_TOWARDS_VIEWER)
 * before it is made a unit vector; its share towards the viewer is how open
 * the ring looks. RING_LONG_AXIS_COSINE and RING_LONG_AXIS_SLOPE turn a
 * direction of the map into the frame of the long axis of the ring.
 */
inline constexpr double RING_OUTER_RADII = 2.0;
inline constexpr double RING_AXIS_ACROSS = 0.5;
inline constexpr double RING_AXIS_DOWN = 0.866;
inline constexpr double RING_AXIS_LEAN = 0.835;
inline constexpr double RING_AXIS_TOWARDS_VIEWER = 0.55;
inline constexpr double RING_LONG_AXIS_COSINE = 0.866;
inline constexpr double RING_LONG_AXIS_SLOPE = -0.5;

/** @brief The pixels past the planet and its ring where the dots of the
 * planet thin out, and the gap kept after them before the first server. */
inline constexpr int PLANET_FADE_OUT = 12;
inline constexpr int PLANET_REACH_GAP = 2;

/**
 * @brief How far a server stands past the reach of the planet: SPREAD_GAIN
 * times the square root of its ping in milliseconds, less SPREAD_FLOOR, so
 * close pings spread apart; and the height its direction may span,
 * SPREAD_HALF_HEIGHT pixels each side of the planet, for an angle of
 * MAXIMUM_ANGLE_DEGREES.
 */
inline constexpr double SPREAD_GAIN = 24.5;
inline constexpr double SPREAD_FLOOR = 3.0;
inline constexpr double SPREAD_HALF_HEIGHT = 78.0;
inline constexpr double MAXIMUM_ANGLE_DEGREES = 75.0;
inline constexpr double DEGREES_PER_HALF_TURN = 180.0;
/** @brief How many times the direction of a server is computed again from
 * the reach it gives, as the mockup does. */
inline constexpr int DIRECTION_REFINEMENTS = 2;

/**
 * @brief The direction a server takes: its id hashed by the 64-bit golden
 * ratio, its top ANGLE_HASH_BITS bits read as a share of the span the map
 * shows at its ping, the asin of SPREAD_HALF_HEIGHT over ANGLE_SPAN_PER_PING
 * times its ping.
 */
inline constexpr std::uint64_t GOLDEN_RATIO_64 = 0x9E3779B97F4A7C15ULL;
inline constexpr unsigned ANGLE_HASH_BITS = 24U;
inline constexpr unsigned ANGLE_HASH_SHIFT = 64U - ANGLE_HASH_BITS;
inline constexpr double ANGLE_SPAN_PER_PING = 1.75;

/**
 * @brief The name of a star: LABEL_GAP pixels right of the star, or
 * LABEL_GAP + 1 left of it, from LABEL_RISE pixels above it, in a box
 * LABEL_HEIGHT tall and LABEL_PADDING wider than the name, kept LABEL_MARGIN
 * inside the map.
 */
inline constexpr int LABEL_GAP = 6;
inline constexpr int LABEL_RISE = 5;
inline constexpr int LABEL_HEIGHT = 9;
inline constexpr int LABEL_PADDING = 2;
inline constexpr int LABEL_MARGIN = 2;

/** @brief The rings of equal ping drawn on the map, in milliseconds, and the
 * step of their dashed line, in degrees around the planet. */
inline constexpr std::array<int, 4> PING_RINGS_MILLISECONDS{25, 75, 150, 225};
inline constexpr int PING_RING_STEP_DEGREES = 3;

}  // namespace rtype::client
