#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
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
  /** @throws BiomeMapException when @p file cannot be read as an LDtk
   * project. */
  explicit LdtkProjectFile(const std::filesystem::path& file);
  ~LdtkProjectFile();

  LdtkProjectFile(const LdtkProjectFile&) = delete;
  LdtkProjectFile& operator=(const LdtkProjectFile&) = delete;
  LdtkProjectFile(LdtkProjectFile&&) noexcept;
  LdtkProjectFile& operator=(LdtkProjectFile&&) noexcept;

  /**
   * @returns The tile layer @p layer of every map of the project, in the
   * order the project lists its maps, two triangles per tile.
   * @throws BiomeMapException when a map lacks the layer.
   */
  [[nodiscard]] MapPlane tilePlane(std::string_view layer) const;

  /**
   * @returns The entity layer @p layer of every map of the project, as
   * tilePlane() gives a tile layer: each entity that has a tile is a prop,
   * the rectangle its tile names, its pivot where the entity lies, two
   * triangles each. An entity without a tile shows nothing.
   * @throws BiomeMapException when a map lacks the layer.
   */
  [[nodiscard]] MapPlane propPlane(std::string_view layer) const;

 private:
  std::unique_ptr<ldtk::Project> project_;
  std::string file_;
};

}  // namespace rtype::client
