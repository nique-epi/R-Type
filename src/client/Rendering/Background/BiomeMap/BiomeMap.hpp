#pragma once

#include <cstddef>
#include <filesystem>
#include <span>
#include <vector>
#include "BiomeLayer.hpp"
#include "MapPlane.hpp"

namespace rtype::client {

/**
 * @brief The maps of a biome, read once from its LDtk project: one plane per
 * layer of the biome, each holding that tile layer of every map of the
 * project, in the order the project lists the maps.
 *
 * LDtkLoader reads the project; the tilesets are textures the caller loads.
 */
class BiomeMap {
 public:
  /**
   * @param project The LDtk file of the biome.
   * @param layers The layers of the biome, each naming a tile layer every map
   * of the project has.
   * @throws BiomeMapException when the file cannot be read as an LDtk project
   * or a map lacks one of the @p layers.
   */
  BiomeMap(const std::filesystem::path& project,
           std::span<const BiomeLayer> layers);

  /** @returns The plane of the layer @p layer, in the order of the layers. */
  [[nodiscard]] const MapPlane& plane(std::size_t layer) const;

 private:
  void readProject(const std::filesystem::path& project,
                   std::span<const BiomeLayer> layers);

  std::vector<MapPlane> planes_;
};

}  // namespace rtype::client
