#include "LevelGroundFiles.hpp"
#include <filesystem>
#include <optional>
#include <span>
#include <string>
#include <utility>
#include "BiomeGround.hpp"
#include "ContentFolders.hpp"
#include "GroundConstants.hpp"
#include "GroundReading.hpp"
#include "LuauLevelEntry.hpp"
#include "LuauLevels.hpp"

namespace rtype::game {

namespace {

/** @returns The LDtk project of the biome @p biome, relative to a content
 * folder. */
std::filesystem::path biomeProjectOf(const std::string& biome) {
  return std::filesystem::path{BIOMES_FOLDER} / biome /
         (biome + std::string{BIOME_PROJECT_EXTENSION});
}

/** @returns The ground of the biome @p biome in @p contentFolders; nothing
 * when no folder holds its project or its project holds none. */
std::optional<BiomeGround> groundOfBiome(
    std::span<const std::filesystem::path> contentFolders,
    const std::string& biome) {
  const std::filesystem::path file =
      engine::contentFilesOf(contentFolders, biomeProjectOf(biome)).back();
  if (!std::filesystem::is_regular_file(file)) {
    return std::nullopt;
  }
  return ldtkGroundOf(file);
}

}  // namespace

void readLevelGrounds(LuauLevels& levels,
                      std::span<const std::filesystem::path> contentFolders) {
  for (const std::string& name : levels.names()) {
    LuauLevelEntry& entry = *levels.entryOf(name);
    for (const auto& [biome, band] : entry.bands) {
      std::optional<BiomeGround> ground = groundOfBiome(contentFolders, biome);
      if (ground.has_value()) {
        entry.grounds.emplace(biome, std::move(*ground));
      }
    }
  }
}

}  // namespace rtype::game
