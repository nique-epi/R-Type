#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include "AssetFileLookup.hpp"
#include "BiomeDecor.hpp"
#include "GaussianBlur.hpp"
#include "ISky.hpp"
#include "PixelBatch.hpp"
#include "PixelCanvas.hpp"
#include "SkyLevel.hpp"
#include "Starfield.hpp"
#include "TextureLookup.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief Draws the starfield behind the playfield, the decor of the biome a
 * level shows over it (see BiomeDecor), and a blurred copy of both added on
 * top as a halo when shaders are available.
 *
 * The sky is painted on a PixelCanvas, so its pixels stay square, equal and
 * sharp at every window size. A screen may speed its stars up and shift it
 * (see ISky); the canvas is cleared to SKY_COLOR whole before the shifted
 * paint, so a shift never uncovers the window behind.
 */
class ScrollingBackground : public ISky {
 public:
  /**
   * @param seed Seed of every random draw of the sky.
   * @param textureOf Finds the tilesets of the decor of a biome; it must not
   * be empty.
   * @param fileOf Finds the LDtk project of a biome; it must not be empty.
   * @throws RenderTextureNotCreatedException when the sky or the halo cannot
   * get a render texture.
   */
  ScrollingBackground(std::uint32_t seed, TextureLookup textureOf,
                      const AssetFileLookup& fileOf);

  /** @brief Moves the sky, see Starfield::advance, and the decor of the
   * level, see BiomeDecor::advance. */
  void advance(engine::Duration elapsed);

  void setSpeedFactor(float factor) override;
  void setOffset(sf::Vector2i pixels) override;
  void showLevel(const SkyLevel& level) override;

  /**
   * @brief Draws the sky over the whole playfield.
   *
   * @param target Target whose view shows the playfield in its logical units.
   * @throws RenderTextureNotCreatedException when the enlarged sky cannot get
   * a render texture of the size the target needs.
   */
  void draw(sf::RenderTarget& target);

 private:
  Starfield starfield_;
  BiomeDecor decor_;
  PixelBatch pixels_;
  PixelCanvas sky_;
  GaussianBlur halo_;
  sf::Vector2i offset_;
};

}  // namespace rtype::client
