#pragma once

#include <optional>

namespace rtype::client {

/**
 * @brief Where the server map lays its parts, the picture unscrolled: the top
 * of its subtitle and how wide it may run, how far down the map is moved from
 * its place, the tops of the two rows of chips, and how tall the whole picture
 * is. As the map was drawn at the size the fonts are drawn at, one row of
 * chips and nothing moved; above it, the subtitle on two lines, the map under
 * it, the chips on two rows under the map, and a picture that scrolls.
 */
struct ServerMapFrame {
  int subtitleTop{};
  std::optional<int> subtitleWidth;
  int mapShift{};
  int upperChipsTop{};
  int lowerChipsTop{};
  int height{};
};

}  // namespace rtype::client
