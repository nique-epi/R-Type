#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include "EnlargedPicture.hpp"

namespace rtype::client {

/**
 * @brief A picture of BACKGROUND_SIZE pixels, laid over the whole playfield
 * with square, equal and sharp pixels at every window size.
 *
 * One of its pixels covers BACKGROUND_PIXEL_SIZE logical units. Where the
 * picture is transparent, the target shows through.
 */
class PixelCanvas {
 public:
  /**
   * @brief Starts with a transparent picture, ready to be laid.
   *
   * @throws RenderTextureNotCreatedException when the canvas cannot get its
   * render texture.
   */
  PixelCanvas();

  /**
   * @brief Where the picture is painted, in pixels of the canvas: clear it,
   * draw on it, then display it before show().
   */
  [[nodiscard]] sf::RenderTexture& canvas();

  /**
   * @brief Lays the picture over the whole playfield, as an EnlargedPicture.
   *
   * Painting with the default alpha blend on a canvas cleared to transparent
   * black stores colors already multiplied by their alpha, whether a pixel is
   * opaque, transparent or translucent like the dimmed layer of a dialog, and
   * so are the colors the smoothing mixes at its edges: the picture is laid
   * with premultipliedAlphaBlend().
   *
   * It leaves the picture unchanged and only refreshes the enlarged copy it
   * keeps for the next frames, so a const draw may call it.
   *
   * @param target Target whose view shows the playfield in its logical units.
   * @throws RenderTextureNotCreatedException when the enlarged picture cannot
   * get a render texture of the size the target needs.
   */
  void show(sf::RenderTarget& target) const;

 private:
  sf::RenderTexture canvas_;
  mutable EnlargedPicture enlargedPicture_;
};

}  // namespace rtype::client
