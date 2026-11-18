#pragma once

#include <cstddef>
#include <string>

/** @brief One batch of tiles a RecordingLayerCanvas was asked to draw: the
 * tileset, how many vertices the triangles hold, and the column they are
 * moved to. */
struct RecordedTiles {
  std::string tileset;
  std::size_t vertices{};
  int column{};

  bool operator==(const RecordedTiles&) const = default;
};
