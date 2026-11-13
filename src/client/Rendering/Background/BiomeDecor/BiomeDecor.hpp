#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <cstddef>
#include <optional>
#include <string_view>
#include <vector>
#include "AssetFileLookup.hpp"
#include "Biome.hpp"
#include "BiomeLayer.hpp"
#include "BiomeMap.hpp"
#include "Logger.hpp"
#include "SkyLevel.hpp"
#include "TextureLookup.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/** @returns The rank of the biome @p name among BIOMES; none for a name no
 * biome has. */
[[nodiscard]] std::optional<std::size_t> biomeIndexOf(std::string_view name);

/**
 * @brief The decor of the biome a level shows: the planes of its maps, drawn
 * over the stars from the farthest to the nearest, each moving by its share
 * of the scroll of the level since the biome began.
 *
 * The maps of every biome of BIOMES are read once, when the decor is built; a
 * project that cannot be read is logged and its biome shows the stars alone.
 * The scroll the server tells is kept, then moved on at the speed it tells
 * until the next one comes, so the decor moves between two messages and
 * stops while the scroll of the level is paused. A plane whose tileset is not
 * loaded is not drawn.
 */
class BiomeDecor {
 public:
  /**
   * @param textureOf Finds the tileset of a layer; it must not be empty.
   * @param fileOf Finds the LDtk project of a biome; it must not be empty.
   */
  BiomeDecor(TextureLookup textureOf, const AssetFileLookup& fileOf);

  /** @brief Shows the biome and the scroll of @p level from now. */
  void show(const SkyLevel& level);

  /** @brief Moves the scroll on by @p elapsed at its speed. A negative time
   * moves nothing. */
  void advance(engine::Duration elapsed);

  /** @brief Draws the planes on @p canvas, whose pixels are those of the
   * background. */
  void draw(sf::RenderTarget& canvas) const;

  /** @returns How far the level scrolled as the decor shows it, in logical
   * units. */
  [[nodiscard]] float scrollPosition() const;

  /** @returns Whether the maps of the biome @p name were read. */
  [[nodiscard]] bool hasMapsOf(std::string_view name) const;

 private:
  [[nodiscard]] std::optional<BiomeMap> readMaps(
      const Biome& biome, const AssetFileLookup& fileOf) const;
  [[nodiscard]] float enteredPixels(const BiomeLayer& layer) const;

  TextureLookup textureOf_;
  logging::Logger logger_;
  std::vector<std::optional<BiomeMap>> maps_;
  SkyLevel level_;
};

}  // namespace rtype::client
