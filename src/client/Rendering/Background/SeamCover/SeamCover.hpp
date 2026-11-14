#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>
#include "PixelBatch.hpp"

namespace rtype::client {

/**
 * @returns Whether the planes of a decor behind its nearest one hide the
 * pixel @p fromSeam, counted in columns right of the seam where they begin
 * to show and in rows: every pixel left of the seam, none from
 * SEAM_EDGE_WIDTH columns right of it, and in between a share that falls
 * from one column to the next along BAYER_MATRIX, laid from the seam so the
 * fringe moves with it.
 */
[[nodiscard]] bool hiddenBySeam(sf::Vector2i fromSeam);

/** @returns Whether the seam on the column @p seam hides every pixel
 * @p canvas shows through its view. */
[[nodiscard]] bool hidesTheWholeCanvas(const sf::RenderTarget& canvas,
                                       int seam);

/**
 * @brief Clears to transparent what hiddenBySeam() hides of the planes
 * behind the nearest one of a decor.
 *
 * Its dithered edge is built once, for the height of the background and the
 * side of the dither matrix more, and drawn moved to the seam and to the rows
 * the canvas shows, so the shake of the camera never uncovers it.
 */
class SeamCover {
 public:
  SeamCover();

  /** @brief Clears what the seam on the column @p seam hides on @p canvas,
   * whose view shows pixels of the background. */
  void cover(sf::RenderTarget& canvas, int seam) const;

 private:
  PixelBatch edge_;
};

}  // namespace rtype::client
