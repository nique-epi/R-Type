#pragma once

namespace rtype::client {

/**
 * @brief How a layer of a background moves, towards the left: by a share of
 * the scroll of the level, and by itself.
 */
struct LayerMotion {
  /** The share of the scroll of the level the layer follows: 0 stays where it
   * is, 1 moves with the level, less lags behind it as a far layer does. */
  float parallax{};
  /** The background pixels per second the layer moves by on its own, whether
   * the level scrolls or not. */
  float autoscroll{};
  /** The width, in background pixels, after which the layer shows again what
   * it showed; 0 for a layer that never repeats. */
  float repeat{};
  /** Whether its autoscroll speeds up while the ships warp. */
  bool warps{};
};

}  // namespace rtype::client
