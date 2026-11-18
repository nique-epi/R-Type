#pragma once

#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <string>

/** @brief One frame a RecordingLayerCanvas was asked to draw: the image, how
 * many frames it holds, the frame, and where its top-left corner goes. */
struct RecordedFrame {
  std::string file;
  std::size_t frames{};
  std::size_t frame{};
  sf::Vector2i topLeft;

  bool operator==(const RecordedFrame&) const = default;
};
