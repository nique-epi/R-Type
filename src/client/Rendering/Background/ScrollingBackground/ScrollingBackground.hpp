#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include "GaussianBlur.hpp"
#include "ISky.hpp"
#include "PixelBatch.hpp"
#include "PixelCanvas.hpp"
#include "SkyLight.hpp"
#include "Starfield.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief Draws the starfield behind the playfield, with a blurred copy added
 * on top as a halo when shaders are available.
 *
 * The sky is painted on a PixelCanvas, so its pixels stay square, equal and
 * sharp at every window size. A screen may speed its stars up and shift it
 * (see ISky); the canvas is cleared to SKY_COLOR whole before the shifted
 * paint, so a shift never uncovers the window behind. Each draw says whether
 * its stars twinkle and its comets flicker (see SkyLight), so a change of
 * REDUCE FLASHES shows at the next frame.
 */
class ScrollingBackground : public ISky {
 public:
  /**
   * @throws RenderTextureNotCreatedException when the sky or the halo cannot
   * get a render texture.
   */
  explicit ScrollingBackground(std::uint32_t seed);

  /** @brief Moves the sky, see Starfield::advance. */
  void advance(engine::Duration elapsed);

  void setSpeedFactor(float factor) override;
  void setOffset(sf::Vector2i pixels) override;

  /**
   * @brief Draws the sky over the whole playfield, its stars and comets
   * shining as @p light asks.
   *
   * @param target Target whose view shows the playfield in its logical units.
   * @param light SkyLight::Steady while the player reduced flashes: no star
   * twinkles and no comet tail flickers on this frame.
   * @throws RenderTextureNotCreatedException when the enlarged sky cannot get
   * a render texture of the size the target needs.
   */
  void draw(sf::RenderTarget& target, SkyLight light);

 private:
  Starfield starfield_;
  PixelBatch pixels_;
  PixelCanvas sky_;
  GaussianBlur halo_;
  sf::Vector2i offset_;
};

}  // namespace rtype::client
