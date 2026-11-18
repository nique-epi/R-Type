#pragma once

#include <cstddef>
#include <string_view>

namespace rtype::client {

/** @brief One frame of an image that holds frames of the same size side by
 * side: the asset id of the image, how many frames it holds, and the rank of
 * the frame, from 0. An image shown whole is its one frame. */
struct StripFrame {
  std::string_view file;
  std::size_t frames{1};
  std::size_t frame{};
};

}  // namespace rtype::client
