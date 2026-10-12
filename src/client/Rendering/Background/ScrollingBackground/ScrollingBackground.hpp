#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <cstdint>
#include "GaussianBlur.hpp"
#include "PixelBatch.hpp"
#include "Starfield.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief Draws the starfield behind the playfield, with a blurred copy added
 * on top as a halo when shaders are available.
 *
 * The sky is painted at BACKGROUND_SIZE, enlarged without smoothing by the
 * largest whole factor that fits the playfield on screen, then smoothed over
 * the small remainder only: its pixels stay square, equal and sharp at every
 * window size.
 */
class ScrollingBackground {
 public:
  /**
   * @throws RenderTextureNotCreatedException when the sky or the halo cannot
   * get a render texture.
   */
  explicit ScrollingBackground(std::uint32_t seed);

  /** @brief Moves the sky, see Starfield::advance. */
  void advance(engine::Duration elapsed);

  /**
   * @brief Draws the sky over the whole playfield.
   *
   * @param target Target whose view shows the playfield in its logical units.
   * @throws RenderTextureNotCreatedException when the enlarged sky cannot get
   * a render texture of the size the target needs.
   */
  void draw(sf::RenderTarget& target);

 private:
  /**
   * @brief Gives the enlarged sky the size of the background times the
   * factor, when it does not have it yet.
   */
  void matchEnlargedSky(unsigned int factor);

  Starfield starfield_;
  PixelBatch pixels_;
  sf::RenderTexture sky_;
  sf::RenderTexture enlargedSky_;
  unsigned int enlargedSkyFactor_ = 0;
  GaussianBlur halo_;
};

}  // namespace rtype::client
