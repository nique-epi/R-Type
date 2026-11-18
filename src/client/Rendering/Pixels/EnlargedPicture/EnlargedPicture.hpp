#pragma once

#include <SFML/Graphics/BlendMode.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace rtype::client {

/**
 * @returns The blend that lays colors already multiplied by their alpha (one,
 * one minus source alpha): what a canvas cleared to transparent black holds
 * once painted with the default alpha blend. The default alpha blend would
 * multiply them a second time and darken the edges.
 */
[[nodiscard]] sf::BlendMode premultipliedAlphaBlend();

/**
 * @brief The background enlarged without smoothing by the largest whole
 * factor that fits the playfield on screen, then laid over the playfield
 * smoothed over the small remainder only: its pixels stay square, equal and
 * sharp at every window size.
 */
class EnlargedPicture {
 public:
  /**
   * @brief Gives the picture the size @p target needs, when it does not have
   * it yet.
   *
   * @param target Target whose view shows the playfield in its logical units.
   * @returns The picture, BACKGROUND_SIZE times factor() pixels, to draw the
   * enlarged background on.
   * @throws RenderTextureNotCreatedException when the picture cannot get a
   * render texture of that size.
   */
  [[nodiscard]] sf::RenderTexture& matching(const sf::RenderTarget& target);

  /** @returns How many times the picture enlarges the background, as of the
   * last matching(). */
  [[nodiscard]] unsigned int factor() const;

  /** @brief Displays the picture, then lays it over the whole playfield of
   * @p target with @p blend. */
  void layOver(sf::RenderTarget& target, const sf::BlendMode& blend);

  /** @returns The picture as last laid. */
  [[nodiscard]] const sf::Texture& texture() const;

 private:
  sf::RenderTexture picture_;
  unsigned int factor_ = 0;
};

}  // namespace rtype::client
