#pragma once

namespace rtype::client {

/** @brief Where a layer of a background stands at a moment, as what it shows
 * needs it to move and to paint. */
struct LayerFrame {
  /** The column of the background the origin of the layer lies on: 0 when it
   * began, lower as it moves to the left. */
  int column{};
  /** How many pixels of streak a particle that stays in the layer leaves
   * behind it while the layer warps; 0 at its own speed. */
  int streak{};
};

}  // namespace rtype::client
