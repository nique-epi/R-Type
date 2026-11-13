#include "ScrollMark.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <algorithm>
#include <cstddef>
#include <optional>
#include "IInterfaceCanvas.hpp"
#include "RowWindow.hpp"
#include "ScrollMarkConstants.hpp"

namespace rtype::client {

std::optional<sf::IntRect> scrollThumbOf(sf::IntRect track, RowWindow rows) {
  const std::size_t lastFirst = lastFirstRowOf(rows);
  if (lastFirst == 0) {
    return std::nullopt;
  }
  const auto trackLength = static_cast<std::size_t>(track.size.y);
  const int length = std::min(
      track.size.y,
      std::max(SCROLL_THUMB_MINIMUM_LENGTH,
               static_cast<int>((trackLength * rows.shown) / rows.count)));
  const auto travel = static_cast<std::size_t>(track.size.y - length);
  const int offset =
      static_cast<int>((travel * std::min(rows.first, lastFirst)) / lastFirst);
  return sf::IntRect({track.position.x, track.position.y + offset},
                     {track.size.x, length});
}

void paintScrollMark(IInterfaceCanvas& canvas, sf::IntRect track,
                     RowWindow rows) {
  const std::optional<sf::IntRect> thumb = scrollThumbOf(track, rows);
  if (!thumb.has_value()) {
    return;
  }
  canvas.fill(track, SCROLL_TRACK_COLOR);
  canvas.fill(*thumb, SCROLL_THUMB_COLOR);
}

}  // namespace rtype::client
