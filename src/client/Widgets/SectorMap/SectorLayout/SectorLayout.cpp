#include "SectorLayout.hpp"
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <numbers>
#include <ratio>
#include <vector>
#include "MapView.hpp"
#include "SectorMapConstants.hpp"
#include "StarBearing.hpp"

namespace rtype::client {

namespace {

/** @returns How open the ring looks: the share of its axis towards the viewer,
 * once the axis is a unit vector, as the mockup computes it. */
double ringOpening() {
  const double across = RING_AXIS_ACROSS * RING_AXIS_LEAN;
  const double down = RING_AXIS_DOWN * RING_AXIS_LEAN;
  return RING_AXIS_TOWARDS_VIEWER /
         std::sqrt((across * across) + (down * down) +
                   (RING_AXIS_TOWARDS_VIEWER * RING_AXIS_TOWARDS_VIEWER));
}

double radiansOf(double degrees) {
  return degrees * std::numbers::pi / DEGREES_PER_HALF_TURN;
}

/** @returns The direction, in radians, a server of @p bearing takes once its
 * angle is spread over the height the map leaves at its distance. */
double spreadDirectionOf(StarBearing bearing) {
  double direction = radiansOf(bearing.angleDegrees);
  for (int pass = 0; pass < DIRECTION_REFINEMENTS; ++pass) {
    const double distance =
        reachOf(direction) + spreadDistanceOf(bearing.pingMilliseconds);
    const double widest =
        std::asin(std::min(1.0, SPREAD_HALF_HEIGHT / distance));
    direction = (bearing.angleDegrees / MAXIMUM_ANGLE_DEGREES) * widest;
  }
  return direction;
}

}  // namespace

double reachOf(double angle) {
  const double longHalf =
      RING_OUTER_RADII * static_cast<double>(SECTOR_PLANET_RADIUS);
  const double shortHalf = longHalf * ringOpening();
  const double alongX = std::cos(angle);
  const double alongY = std::sin(angle);
  const double along =
      (alongX * RING_LONG_AXIS_COSINE) - (alongY * RING_LONG_AXIS_SLOPE);
  const double across =
      (alongX * RING_LONG_AXIS_SLOPE) + (alongY * RING_LONG_AXIS_COSINE);
  const double ring =
      1.0 / std::sqrt(((along / longHalf) * (along / longHalf)) +
                      ((across / shortHalf) * (across / shortHalf)));
  return std::max(static_cast<double>(SECTOR_PLANET_RADIUS), ring) +
         PLANET_FADE_OUT + PLANET_REACH_GAP;
}

double spreadDistanceOf(double pingMilliseconds) {
  return std::max(0.0,
                  SPREAD_GAIN * (std::sqrt(pingMilliseconds) - SPREAD_FLOOR));
}

sf::Vector2i placeOf(StarBearing bearing, const MapView& view) {
  const double direction = spreadDirectionOf(bearing);
  const double distance =
      reachOf(direction) + spreadDistanceOf(bearing.pingMilliseconds);
  return {static_cast<int>(std::lround(
              view.centerX + (distance * std::cos(direction) * view.zoom))),
          static_cast<int>(std::lround(
              view.centerY + (distance * std::sin(direction) * view.zoom)))};
}

MapView viewOn(StarBearing bearing, double zoom, sf::Vector2i target) {
  const sf::Vector2i fromCenter =
      placeOf(bearing, MapView{.zoom = zoom, .centerX = 0.0, .centerY = 0.0});
  return MapView{.zoom = zoom,
                 .centerX = static_cast<double>(target.x - fromCenter.x),
                 .centerY = static_cast<double>(target.y - fromCenter.y)};
}

int angleOf(std::int64_t serverId,
            std::chrono::duration<double, std::milli> ping) {
  const double span = std::min(
      MAXIMUM_ANGLE_DEGREES,
      std::asin(std::min(
          1.0, SPREAD_HALF_HEIGHT / (ping.count() * ANGLE_SPAN_PER_PING))) *
          DEGREES_PER_HALF_TURN / std::numbers::pi);
  const std::uint64_t hashed =
      static_cast<std::uint64_t>(serverId) * GOLDEN_RATIO_64;
  const double share = static_cast<double>(hashed >> ANGLE_HASH_SHIFT) /
                       static_cast<double>(std::uint64_t{1} << ANGLE_HASH_BITS);
  return static_cast<int>(std::lround(-span + (share * (span + span))));
}

std::vector<sf::Vector2f> pingContourOf(double pingMilliseconds,
                                        const MapView& view) {
  std::vector<sf::Vector2f> points;
  const int quarterTurn = static_cast<int>(DEGREES_PER_HALF_TURN) / 2;
  for (int degrees = -quarterTurn; degrees <= quarterTurn;
       degrees += PING_RING_STEP_DEGREES) {
    const double direction = radiansOf(degrees);
    const double distance =
        (reachOf(direction) + spreadDistanceOf(pingMilliseconds)) * view.zoom;
    points.emplace_back(
        static_cast<float>(view.centerX + (distance * std::cos(direction))),
        static_cast<float>(view.centerY + (distance * std::sin(direction))));
  }
  return points;
}

}  // namespace rtype::client
