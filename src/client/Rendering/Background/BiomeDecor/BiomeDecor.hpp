#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <span>
#include <string_view>
#include <vector>
#include "BiomeLayer.hpp"
#include "SkyLevel.hpp"
#include "TextureLookup.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/** @returns The layers of the biome @p name among BIOMES; none for a name
 * no biome has. */
[[nodiscard]] std::span<const BiomeLayer> layersOfBiome(std::string_view name);

/**
 * @returns The columns of the sky, in background pixels, where the copies of
 * a picture @p tileWidth pixels wide start once it scrolled @p scrolledPixels
 * to the left, from the one that covers the left edge to the one that covers
 * the right edge; none for a picture without width.
 */
[[nodiscard]] std::vector<int> decorTileColumns(float scrolledPixels,
                                                int tileWidth);

/**
 * @brief The decor of the biome a level shows: its layers, drawn over the
 * stars from the farthest to the nearest, each moving by its share of the
 * scroll of the level.
 *
 * The scroll the server tells is kept, then moved on at the speed it tells
 * until the next one comes, so the decor moves between two messages and
 * stops while the scroll of the level is paused. A layer whose picture is not
 * loaded is not drawn, and a biome the client has no decor for shows none.
 */
class BiomeDecor {
 public:
  /** @param textureOf Finds the picture of a layer; it must not be empty. */
  explicit BiomeDecor(TextureLookup textureOf);

  /** @brief Shows the biome and the scroll of @p level from now. */
  void show(const SkyLevel& level);

  /** @brief Moves the scroll on by @p elapsed at its speed. A negative time
   * moves nothing. */
  void advance(engine::Duration elapsed);

  /** @brief Draws the layers on @p canvas, whose pixels are those of the
   * background. */
  void draw(sf::RenderTarget& canvas) const;

  /** @returns How far the level scrolled as the decor shows it, in logical
   * units. */
  [[nodiscard]] float scrollPosition() const;

 private:
  TextureLookup textureOf_;
  SkyLevel level_;
};

}  // namespace rtype::client
