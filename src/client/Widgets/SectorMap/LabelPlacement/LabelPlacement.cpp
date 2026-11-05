#include "LabelPlacement.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <algorithm>
#include <span>
#include <vector>
#include "LabelCandidate.hpp"
#include "LabelSide.hpp"
#include "SectorMapConstants.hpp"

namespace rtype::client {

namespace {

sf::IntRect rightBoxOf(const LabelCandidate& star) {
  return sf::IntRect{{star.point.x + LABEL_GAP, star.point.y - LABEL_RISE},
                     {star.nameWidth + LABEL_PADDING, LABEL_HEIGHT}};
}

sf::IntRect leftBoxOf(const LabelCandidate& star) {
  const int width = star.nameWidth + LABEL_PADDING;
  return sf::IntRect{
      {star.point.x - LABEL_GAP - 1 - width, star.point.y - LABEL_RISE},
      {width, LABEL_HEIGHT}};
}

/** @returns Whether @p box lies inside the map, within LABEL_MARGIN, across:
 * the mockup bounds a name from left to right only. */
bool fitsTheMap(sf::IntRect box) {
  return box.position.x >= SECTOR_MAP_AREA.position.x + LABEL_MARGIN &&
         box.position.x + box.size.x <=
             SECTOR_MAP_AREA.position.x + SECTOR_MAP_AREA.size.x - LABEL_MARGIN;
}

bool isFree(sf::IntRect box, const std::vector<sf::IntRect>& taken) {
  return fitsTheMap(box) &&
         std::ranges::none_of(taken, [box](const sf::IntRect& other) {
           return box.findIntersection(other).has_value();
         });
}

}  // namespace

std::vector<LabelSide> placeLabels(std::span<const LabelCandidate> stars,
                                   std::span<const sf::IntRect> forbidden) {
  std::vector<sf::IntRect> taken(forbidden.begin(), forbidden.end());
  std::vector<LabelSide> sides;
  sides.reserve(stars.size());
  for (const LabelCandidate& star : stars) {
    if (const sf::IntRect right = rightBoxOf(star); isFree(right, taken)) {
      taken.push_back(right);
      sides.push_back(LabelSide::Right);
    } else if (const sf::IntRect left = leftBoxOf(star); isFree(left, taken)) {
      taken.push_back(left);
      sides.push_back(LabelSide::Left);
    } else {
      sides.push_back(LabelSide::None);
    }
  }
  return sides;
}

}  // namespace rtype::client
