#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include "MapLayerKind.hpp"
#include "MapPlane.hpp"

namespace ldtk {
class Project;
}  // namespace ldtk

namespace rtype::client {

/**
 * @brief An LDtk project, read once with LDtkLoader, from which the layers
 * of a background take the maps they show: several layers of one background
 * usually name the same project.
 */
class LdtkProjectFile {
 public:
  /** @throws LdtkProjectException when @p file cannot be read as an LDtk
   * project. */
  explicit LdtkProjectFile(const std::filesystem::path& file);
  ~LdtkProjectFile();

  LdtkProjectFile(const LdtkProjectFile&) = delete;
  LdtkProjectFile& operator=(const LdtkProjectFile&) = delete;
  LdtkProjectFile(LdtkProjectFile&&) noexcept;
  LdtkProjectFile& operator=(LdtkProjectFile&&) noexcept;

  /**
   * @returns The layer @p layer of every map of the project, in the order
   * the project lists its maps, read as @p kind says. A layer of tiles gives
   * two triangles per tile. A layer of props gives two triangles per entity
   * that has a tile: the rectangle its tile names, its pivot where the entity
   * lies; an entity without a tile shows nothing.
   * @throws LdtkProjectException when a map lacks the layer.
   */
  [[nodiscard]] MapPlane plane(std::string_view layer, MapLayerKind kind) const;

 private:
  std::unique_ptr<ldtk::Project> project_;
  std::string file_;
};

}  // namespace rtype::client
