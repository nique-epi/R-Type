#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <cstddef>
#include <optional>
#include <string_view>
#include <vector>
#include "AssetFileLookup.hpp"
#include "Biome.hpp"
#include "BiomeMap.hpp"
#include "Logger.hpp"
#include "SeamCover.hpp"
#include "SkyLevel.hpp"
#include "TextureLookup.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/** @returns The rank of the biome @p name among BIOMES; none for a name no
 * biome has. */
[[nodiscard]] std::optional<std::size_t> biomeIndexOf(std::string_view name);

/**
 * @brief The decor of the biome a level shows: the planes of its maps, from
 * the farthest to the nearest, each moving by its share of the scroll of the
 * level since the biome began, and each drawn apart, so a slow plane can
 * move by less than a pixel.
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

  /**
   * @brief Draws the plane @p layer of the biome shown on @p canvas, whose
   * view shows pixels of the background.
   *
   * The nearest plane slides in from the right edge of the sky. The planes
   * behind it cover the whole sky from the start, but only show right of
   * seamColumn(), fading in there (see SeamCover): the sky shows one seam,
   * which moves with the nearest plane. A plane is drawn on whole pixels,
   * layerShift() right of where it lies.
   *
   * @returns Whether it drew anything: not when the biome has no such plane,
   * its maps or its tileset are missing, or the seam hides the whole plane.
   */
  [[nodiscard]] bool drawLayer(std::size_t layer,
                               sf::RenderTarget& canvas) const;

  /**
   * @returns How far right of the pixels drawLayer() uses the plane @p layer
   * lies, from -1 excluded to 0 included; 0 for the nearest plane, which
   * keeps the grid of the playfield, and for a plane the biome lacks.
   */
  [[nodiscard]] float layerShift(std::size_t layer) const;

  /**
   * @returns The column of the sky where the planes behind the nearest begin
   * to show: the first tile of the first map of the nearest plane, where the
   * entrance of the biome begins. None when the biome shown has no maps.
   */
  [[nodiscard]] std::optional<int> seamColumn() const;

  /** @returns How far the level scrolled as the decor shows it, in logical
   * units. */
  [[nodiscard]] float scrollPosition() const;

  /** @returns Whether the maps of the biome @p name were read. */
  [[nodiscard]] bool hasMapsOf(std::string_view name) const;

 private:
  [[nodiscard]] std::optional<BiomeMap> readMaps(
      const Biome& biome, const AssetFileLookup& fileOf) const;

  TextureLookup textureOf_;
  logging::Logger logger_;
  std::vector<std::optional<BiomeMap>> maps_;
  SkyLevel level_;
  SeamCover seamCover_;
};

}  // namespace rtype::client
