#include "RingedPlanet.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>
#include "BackgroundColorConstants.hpp"
#include "BayerThreshold.hpp"
#include "ColoredPixel.hpp"
#include "DitherConstants.hpp"
#include "LandWave.hpp"
#include "MotionPreference.hpp"
#include "PlanetPose.hpp"
#include "RingedPlanetConstants.hpp"
#include "SectorMapConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @brief A point or a direction of the space of the planet: x to the right,
 * y down, z towards the viewer, in radii of the planet. */
struct SpacePoint {
  double x{};
  double y{};
  double z{};
};

double dot(SpacePoint first, SpacePoint second) {
  return (first.x * second.x) + (first.y * second.y) + (first.z * second.z);
}

SpacePoint unit(SpacePoint point) {
  const double length = std::sqrt(dot(point, point));
  return SpacePoint{
      .x = point.x / length, .y = point.y / length, .z = point.z / length};
}

SpacePoint cross(SpacePoint left, SpacePoint right) {
  return SpacePoint{.x = (left.y * right.z) - (left.z * right.y),
                    .y = (left.z * right.x) - (left.x * right.z),
                    .z = (left.x * right.y) - (left.y * right.x)};
}

/** @brief The axis of the planet and two directions of its equator, from
 * which the longitude and the angle around the ring are read. */
struct EquatorFrame {
  SpacePoint axis;
  SpacePoint first;
  SpacePoint second;
};

EquatorFrame equatorFrame() {
  const SpacePoint axis =
      unit(SpacePoint{.x = RING_AXIS_ACROSS * RING_AXIS_LEAN,
                      .y = -RING_AXIS_DOWN * RING_AXIS_LEAN,
                      .z = RING_AXIS_TOWARDS_VIEWER});
  const SpacePoint first =
      unit(cross(axis, SpacePoint{.x = 0.0, .y = 0.0, .z = 1.0}));
  return EquatorFrame{
      .axis = axis, .first = first, .second = cross(axis, first)};
}

/** @returns The direction of the sun, made a unit vector once. */
const SpacePoint& sunDirection() {
  static const SpacePoint sun =
      unit(SpacePoint{.x = SUN_ACROSS, .y = SUN_DOWN, .z = SUN_TOWARDS_VIEWER});
  return sun;
}

/** @returns A value from 0 to 1 that is the same for the same cell at every
 * frame, and looks random from one cell to the next. */
double cellNoise(int column, int row) {
  const double wave =
      std::sin((column * NOISE_ACROSS) + (row * NOISE_DOWN)) * NOISE_SCALE;
  return wave - std::floor(wave);
}

/** @brief A point of the surface of the planet, in radians. */
struct SurfacePoint {
  double longitude{};
  double latitude{};
};

double landAt(SurfacePoint point) {
  double land = LAND_LEVEL;
  for (const LandWave& wave : LAND_WAVES) {
    const double bent = wave.latitudeBend == 0.0
                            ? point.latitude
                            : std::sin(wave.latitudeBend * point.latitude);
    land += wave.weight * std::sin((wave.longitude * point.longitude) +
                                   (wave.latitude * bent) + wave.phase);
  }
  return land;
}

/** @returns How dense the ring is at @p radius radii: nothing off the ring,
 * little in its gap, then its two bands, each fading away from the gap. */
double ringDensity(double radius) {
  if (radius < RING_INNER_RADII || radius > RING_OUTER_RADII) {
    return 0.0;
  }
  if (radius > RING_GAP_START && radius < RING_GAP_END) {
    return RING_GAP_DENSITY;
  }
  const double band =
      radius < RING_GAP_START
          ? RING_INNER_BAND_TOP -
                (RING_INNER_BAND_FALL * (RING_GAP_START - radius) /
                 (RING_GAP_START - RING_INNER_RADII))
          : RING_OUTER_BAND_TOP -
                (RING_OUTER_BAND_FALL * (radius - RING_GAP_END) /
                 (RING_OUTER_RADII - RING_GAP_END));
  return band *
         ((1.0 - RING_BAND_RIPPLE) +
          (RING_BAND_RIPPLE * std::sin(radius * RING_BAND_RIPPLES_PER_RADIUS)));
}

/** @brief A cell of the halftone: where its ray crosses the plane of the view,
 * in radii, its column and row, and its Bayer threshold. */
struct HalftoneCell {
  double x{};
  double y{};
  int column{};
  int row{};
  double threshold{};
};

/** @brief What a cell of the ring holds: its light, and how close to the
 * viewer the ring is there. */
struct RingPoint {
  double light{};
  double depth{};
};

/** @returns The ring where the ray of @p cell crosses the equator, nothing
 * off the ring or for a ring seen edge on. */
std::optional<RingPoint> ringAt(const HalftoneCell& cell,
                                const EquatorFrame& frame, double drift) {
  const SpacePoint axis = frame.axis;
  if (std::abs(axis.z) <= RING_AXIS_EDGE_ON) {
    return std::nullopt;
  }
  const SpacePoint point{
      .x = cell.x,
      .y = cell.y,
      .z = -((axis.x * cell.x) + (axis.y * cell.y)) / axis.z};
  const double radius = std::sqrt(dot(point, point));
  const double density = ringDensity(radius);
  if (density <= 0.0) {
    return std::nullopt;
  }
  const double around =
      std::atan2(dot(point, frame.second), dot(point, frame.first)) + drift;
  const double grains =
      RING_GRAIN_BASE +
      (RING_GRAIN_SPREAD *
       std::sin((RING_GRAIN_TURNS_A * around) + (radius * RING_GRAIN_RADII_A)) *
       std::sin((RING_GRAIN_TURNS_B * around) - (radius * RING_GRAIN_RADII_B)));
  const SpacePoint sun = sunDirection();
  const double towardsSun = dot(point, sun);
  const SpacePoint offAxis{.x = point.x - (towardsSun * sun.x),
                           .y = point.y - (towardsSun * sun.y),
                           .z = point.z - (towardsSun * sun.z)};
  const bool shaded = towardsSun < 0.0 && dot(offAxis, offAxis) < 1.0;
  return RingPoint{
      .light = density * grains * (shaded ? RING_SHADOW_LIGHT : 1.0),
      .depth = point.z};
}

/** @returns The light of the sphere at the point of @p normal, its lands
 * turned by @p spin. */
double sphereLight(SpacePoint normal, const EquatorFrame& frame, double spin) {
  const double facing = dot(normal, sunDirection());
  const double diffuse =
      std::max(0.0, (facing + LIGHT_WRAP) / (1.0 + LIGHT_WRAP));
  const double rim = std::pow(1.0 - normal.z, RIM_SHARPNESS) *
                     std::max(0.0, facing + RIM_NIGHT_MARGIN) * LIGHT_RIM;
  const double latitude =
      std::asin(std::clamp(dot(normal, frame.axis), -1.0, 1.0));
  const double longitude =
      std::atan2(dot(normal, frame.second), dot(normal, frame.first)) + spin;
  return LIGHT_AMBIENT +
         (diffuse * diffuse *
          (LIGHT_OF_THE_SEA +
           (LIGHT_OF_THE_LAND *
            landAt({.longitude = longitude, .latitude = latitude})))) +
         rim;
}

/** @brief What a cell shows: a dot of the ring or of the sphere, and its
 * light; a thin dot keeps one pixel. */
struct HalftoneDot {
  double light{};
  bool ofTheRing{};
  bool thin{};
};

HalftoneDot ringDot(const RingPoint& ring) {
  return HalftoneDot{.light = ring.light, .ofTheRing = true, .thin = true};
}

/** @returns The dot of the sphere at @p cell, nothing where the dots of its
 * edge thinned out. */
std::optional<HalftoneDot> sphereDot(const HalftoneCell& cell,
                                     const PlanetPose& pose,
                                     const EquatorFrame& frame) {
  const double distance = std::sqrt((cell.x * cell.x) + (cell.y * cell.y));
  const double outside = (distance - 1.0) * pose.radius;
  const double keep = outside < -PLANET_FADE_IN
                          ? 1.0
                          : 1.0 - ((outside + PLANET_FADE_IN) /
                                   (PLANET_FADE_IN + PLANET_FADE_OUT));
  if (cellNoise(cell.column, cell.row) > keep) {
    return std::nullopt;
  }
  const SpacePoint normal =
      distance <= 1.0
          ? SpacePoint{.x = cell.x,
                       .y = cell.y,
                       .z = std::sqrt(1.0 - (distance * distance))}
          : SpacePoint{
                .x = cell.x / distance, .y = cell.y / distance, .z = 0.0};
  return HalftoneDot{.light = sphereLight(normal, frame, pose.spin),
                     .ofTheRing = false,
                     .thin = keep < HALFTONE_SMALL_KEEP};
}

/** @returns What @p cell shows, as the mockup decides between the ring and the
 * sphere: the ring alone off the sphere, the ring in front of the sphere as
 * often as it is dense, the sphere otherwise, its edge thinning into the ring
 * or the sky. */
std::optional<HalftoneDot> dotAt(const HalftoneCell& cell,
                                 const PlanetPose& pose,
                                 const EquatorFrame& frame) {
  const std::optional<RingPoint> ring = ringAt(cell, frame, pose.drift);
  const double squared = (cell.x * cell.x) + (cell.y * cell.y);
  const double outside = (std::sqrt(squared) - 1.0) * pose.radius;
  const bool onSphere = squared <= 1.0;
  if (ring.has_value() &&
      (!onSphere || (ring->depth > std::sqrt(1.0 - squared) &&
                     cellNoise(cell.row + RING_NOISE_ROW_SHIFT,
                               cell.column + RING_NOISE_COLUMN_SHIFT) <
                         ring->light * RING_FRONT_COVER))) {
    return ringDot(*ring);
  }
  if (!onSphere && outside >= PLANET_FADE_OUT) {
    return std::nullopt;
  }
  const std::optional<HalftoneDot> sphere = sphereDot(cell, pose, frame);
  if (sphere.has_value() || !ring.has_value()) {
    return sphere;
  }
  return ringDot(*ring);
}

/** @returns The color a dot of the ring takes, nothing for one too dark to
 * show: pale in the sun, blue in the shade, deep in its gap. */
std::optional<sf::Color> ringColorOf(const HalftoneDot& dot,
                                     const HalftoneCell& cell) {
  const double tone =
      dot.light + ((cell.threshold - BAYER_CELL_CENTRE) * RING_TONE_SPREAD);
  if (tone < RING_DARKEST_TONE) {
    return std::nullopt;
  }
  if (tone > RING_PALE_TONE) {
    return PALE_BLUE;
  }
  return tone > RING_BLUE_TONE ? BLUE : DEEP_BLUE;
}

/** @brief Adds a square of @p size pixels of @p color at @p corner. */
void plotSquare(std::vector<ColoredPixel>& pixels, sf::Vector2i corner,
                int size, sf::Color color) {
  for (int row = 0; row < size; ++row) {
    for (int column = 0; column < size; ++column) {
      pixels.push_back(ColoredPixel{
          .position = corner + sf::Vector2i{column, row}, .color = color});
    }
  }
}

/** @brief Adds the pixels of @p dot of @p cell at @p corner. */
void plotDot(std::vector<ColoredPixel>& pixels, sf::Vector2i corner,
             const HalftoneDot& dot, const HalftoneCell& cell) {
  if (dot.ofTheRing) {
    if (const std::optional<sf::Color> color = ringColorOf(dot, cell)) {
      plotSquare(pixels, corner, 1, *color);
    }
    return;
  }
  const int level = std::min(
      DOT_LEVEL_COUNT, static_cast<int>(std::floor(
                           (dot.light * DOT_LEVEL_COUNT) + cell.threshold)));
  if (level <= 0) {
    return;
  }
  const auto index = static_cast<std::size_t>(level - 1);
  plotSquare(pixels, corner, dot.thin ? 1 : DOT_SIZES.at(index),
             DOT_COLORS.at(index));
}

/** @returns The cell of the halftone at @p column and @p row of the picture of
 * the planet, whose centre is @p half pixels from its corner. */
HalftoneCell cellAt(sf::Vector2i pixel, int half, int radius) {
  return HalftoneCell{.x = static_cast<double>(pixel.x + 1 - half) / radius,
                      .y = static_cast<double>(pixel.y + 1 - half) / radius,
                      .column = pixel.x,
                      .row = pixel.y,
                      .threshold = bayerThresholdAt(pixel / HALFTONE_PITCH)};
}

/** @returns The half size of the picture of the planet: its ring, then the
 * fade of its dots and PLANET_REACH_GAP. */
int pictureHalfOf(int radius) {
  return static_cast<int>(std::ceil(RING_OUTER_RADII * radius)) +
         PLANET_FADE_OUT + PLANET_REACH_GAP;
}

/** @returns How many whole frames of PLANET_FRAMES_PER_SECOND fit in @p
 * span, counted in whole numbers so no rounding of a frame builds up. */
std::int64_t framesIn(engine::Duration span) {
  return span.count() * PLANET_FRAMES_PER_SECOND /
         std::chrono::duration_cast<engine::Duration>(std::chrono::seconds{1})
             .count();
}

/** @returns The angle a turn of @p period has reached after @p frame frames. */
double turnAfter(std::int64_t frame, std::chrono::milliseconds period) {
  const std::int64_t framesPerTurn = framesIn(period);
  return FULL_TURN_RADIANS * static_cast<double>(frame % framesPerTurn) /
         static_cast<double>(framesPerTurn);
}

/** @returns Whether a dot whose corner is @p corner may paint inside @p clip:
 * a dot is at most HALFTONE_PITCH - 1 pixels wide, so a cell whose square
 * misses the clip is never shaded. */
bool reachesTheClip(sf::Vector2i corner, sf::IntRect clip) {
  const int widest = HALFTONE_PITCH - 1;
  return clip.findIntersection(sf::IntRect{corner, {widest, widest}})
      .has_value();
}

}  // namespace

std::vector<ColoredPixel> ringedPlanetPixels(PlanetPose pose,
                                             sf::IntRect clip) {
  const int half = pictureHalfOf(pose.radius);
  const int size = (2 * half) + 1;
  const sf::Vector2i origin = pose.center - sf::Vector2i{half, half};
  const EquatorFrame frame = equatorFrame();
  std::vector<ColoredPixel> pixels;
  for (int row = 0; row < size; row += HALFTONE_PITCH) {
    for (int column = 0; column < size; column += HALFTONE_PITCH) {
      const sf::Vector2i corner = origin + sf::Vector2i{column, row};
      if (!reachesTheClip(corner, clip)) {
        continue;
      }
      const HalftoneCell cell = cellAt({column, row}, half, pose.radius);
      if (const std::optional<HalftoneDot> found = dotAt(cell, pose, frame)) {
        plotDot(pixels, corner, *found, cell);
      }
    }
  }
  std::erase_if(pixels, [clip](const ColoredPixel& pixel) {
    return !clip.contains(pixel.position);
  });
  return pixels;
}

PlanetPose turnedPlanet(PlanetPose pose, engine::Duration elapsed,
                        MotionPreference motion) {
  if (motion == MotionPreference::Reduced) {
    return pose;
  }
  const std::int64_t frame = framesIn(elapsed);
  pose.spin = turnAfter(frame, PLANET_TURN);
  pose.drift = turnAfter(frame, RING_TURN);
  return pose;
}

}  // namespace rtype::client
