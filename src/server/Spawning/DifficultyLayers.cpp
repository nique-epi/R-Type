#include "DifficultyLayers.hpp"
#include <cstddef>
#include <cstdint>
#include <ranges>
#include <span>
#include <string>
#include <string_view>
#include <toml++/toml.hpp>
#include "DataFileReading.hpp"
#include "DataFileTable.hpp"
#include "ProtocolConstants.hpp"
#include "ServerException.hpp"
#include "WaveFileConstants.hpp"
#include "WholeNumberRange.hpp"
#include "WireTypes.hpp"

namespace rtype::server {

namespace {

using engine::DataFileTable;

/** @throws InvalidDifficultyNameException, at the table, when the protocol
 * cannot carry @p name, a table of @p levels: the rules of a nickname. */
void requireDifficultyName(const DataFileTable& levels, std::string_view name) {
  if (!network::isValidNickname(name)) {
    throw InvalidDifficultyNameException(
        engine::placeOfKey(levels, name),
        engine::WholeNumberRange{
            .lowest = static_cast<std::int64_t>(network::MINIMUM_NICKNAME_SIZE),
            .highest =
                static_cast<std::int64_t>(network::MAXIMUM_NICKNAME_SIZE)});
  }
}

/** @throws TooManyDifficultiesException, at the table of difficulties of
 * @p root, when @p count, the difficulties laid once its file is laid,
 * exceeds network::MAXIMUM_DIFFICULTIES. */
void requireFewDifficulties(const DataFileTable& root, std::size_t count) {
  if (count > network::MAXIMUM_DIFFICULTIES) {
    throw TooManyDifficultiesException(
        engine::placeOfKey(root, DIFFICULTY_TABLE_KEY),
        network::MAXIMUM_DIFFICULTIES);
  }
}

/** @returns The table of difficulties of the file whose root is @p root, the
 * keys of the root checked. */
DataFileTable levelsOf(const DataFileTable& root) {
  engine::refuseUnknownKeys(root, ROOT_KEYS);
  return engine::tableAt(root, DIFFICULTY_TABLE_KEY);
}

/** @returns The table of the difficulty @p name of @p levels, its name and
 * its keys checked. */
DataFileTable checkedLevel(const DataFileTable& levels, std::string_view name) {
  requireDifficultyName(levels, name);
  DataFileTable level = engine::tableAt(levels, name);
  engine::refuseUnknownKeys(level, WAVE_PLAN_KEYS);
  return level;
}

/** @brief Lays the difficulties of the file whose root is @p root over
 * @p laid, each checked before it is laid. */
void lay(const DataFileTable& root, LayersByDifficulty& laid) {
  const DataFileTable levels = levelsOf(root);
  for (auto&& entry : levels.table.get()) {
    const std::string_view name = entry.first.str();
    laid[std::string{name}].push_back(checkedLevel(levels, name));
  }
}

}  // namespace

LayersByDifficulty difficultyLayersOf(std::span<const DataFileTable> roots) {
  LayersByDifficulty laid;
  for (const DataFileTable& root : roots) {
    lay(root, laid);
    requireFewDifficulties(root, laid.size());
  }
  return laid;
}

const DataFileTable& layerHolding(const DifficultyLayers& layers,
                                  std::string_view key) {
  for (const DataFileTable& layer : layers | std::views::reverse) {
    if (layer.table.get().contains(key)) {
      return layer;
    }
  }
  return layers.front();
}

}  // namespace rtype::server
