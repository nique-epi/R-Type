#include "StarNavigation.hpp"
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <cstdlib>
#include <optional>
#include <span>
#include "MapDirection.hpp"

namespace rtype::client {

namespace {

/** @returns One step towards @p direction, on the grid where y goes down. */
sf::Vector2i stepOf(MapDirection direction) {
  switch (direction) {
    case MapDirection::Up:
      return {0, -1};
    case MapDirection::Down:
      return {0, 1};
    case MapDirection::Left:
      return {-1, 0};
    case MapDirection::Right:
      break;
  }
  return {1, 0};
}

/** @returns Whether @p offset leads towards @p step, 45 degrees at most each
 * side: ahead along it by at least as much as aside from it. */
bool liesTowards(sf::Vector2i offset, sf::Vector2i step) {
  const int ahead = (offset.x * step.x) + (offset.y * step.y);
  const int aside = std::abs((offset.x * step.y) - (offset.y * step.x));
  return ahead > 0 && aside <= ahead;
}

}  // namespace

std::optional<std::size_t> neighbourOf(std::span<const sf::Vector2i> stars,
                                       std::size_t from,
                                       MapDirection direction) {
  if (from >= stars.size()) {
    return std::nullopt;
  }
  const sf::Vector2i step = stepOf(direction);
  std::optional<std::size_t> nearest;
  int nearestDistance = 0;
  for (std::size_t index = 0; index < stars.size(); ++index) {
    const sf::Vector2i offset = stars[index] - stars[from];
    const int distance = (offset.x * offset.x) + (offset.y * offset.y);
    if (index != from && liesTowards(offset, step) &&
        (!nearest.has_value() || distance < nearestDistance)) {
      nearest = index;
      nearestDistance = distance;
    }
  }
  return nearest;
}

}  // namespace rtype::client
