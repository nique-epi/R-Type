#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderTexture.hpp>

namespace rtype::client {

/**
 * @brief A picture of BACKGROUND_SIZE pixels, laid over the whole playfield
 * with square, equal and sharp pixels at every window size.
 *
 * One of its pixels covers BACKGROUND_PIXEL_SIZE logical units. To be shown,
 * the picture is enlarged without smoothing by the largest whole factor that
 * fits the playfield on screen, then smoothed over the small remainder only.
 * Where the picture is transparent, the target shows through.
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
   * @brief Lays the picture over the whole playfield.
   *
   * Painting with the default alpha blend on a canvas cleared to transparent
   * black stores colors already multiplied by their alpha, whether a pixel is
   * opaque, transparent or translucent like the dimmed layer of a dialog,
   * and so are the colors the smoothing mixes at its edges. It is therefore
   * laid with the blend that expects such colors (one, one minus source alpha):
   * the default alpha blend would multiply them a second time and darken the
   * edges.
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
  /**
   * @brief Gives the enlarged picture the size of the canvas times the factor,
   * when it does not have it yet.
   */
  void matchEnlargedPicture(unsigned int factor) const;

  sf::RenderTexture canvas_;
  mutable sf::RenderTexture enlargedPicture_;
  mutable unsigned int enlargedPictureFactor_ = 0;
};

}  // namespace rtype::client
