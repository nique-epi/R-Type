#include "WaveCatalog.hpp"
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <map>
#include <optional>
#include <ranges>
#include <span>
#include <string>
#include <string_view>
#include <toml++/toml.hpp>
#include <utility>
#include <vector>
#include "BydoKind.hpp"
#include "BydoWave.hpp"
#include "DataFileReading.hpp"
#include "DataFileTable.hpp"
#include "ServerException.hpp"
#include "SipHash.hpp"
#include "WaveCurve.hpp"
#include "WaveFileConstants.hpp"
#include "WavePlan.hpp"
#include "WholeNumberRange.hpp"

namespace rtype::server {

namespace {

using PlansByDifficulty = std::map<std::string, WavePlan, std::less<>>;
using engine::DataFileTable;

/** @returns Whether @p count is within @p range. */
bool countFits(std::size_t count, engine::WholeNumberRange range) {
  const auto signedCount = static_cast<std::int64_t>(count);
  return signedCount >= range.lowest && signedCount <= range.highest;
}

/** @returns The count @p key of @p table holds; throws as
 * engine::wholeNumberAt(). */
std::size_t countAt(const DataFileTable& table, std::string_view key,
                    engine::WholeNumberRange range) {
  return static_cast<std::size_t>(engine::wholeNumberAt(table, key, range));
}

/** @returns The names of ENEMY_NAMES, for the errors. */
std::vector<std::string> enemyNames() {
  std::vector<std::string> names;
  names.reserve(ENEMY_NAMES.size());
  for (const auto& entry : ENEMY_NAMES) {
    names.emplace_back(entry.first);
  }
  return names;
}

/**
 * @returns The kind @p name, an element of the enemies of @p table, names.
 * @throws InvalidEnemyListException when it is not a string, and
 * UnknownEnemyException when it is no name of ENEMY_NAMES.
 */
game::BydoKind kindNamed(const toml::node& name, const DataFileTable& table) {
  const std::optional<std::string> text = name.value_exact<std::string>();
  if (!text.has_value()) {
    throw InvalidEnemyListException(
        engine::placeOf(name.source(), table, ENEMIES_KEY), ENEMY_LIST_LENGTH);
  }
  for (const auto& [enemyName, kind] : ENEMY_NAMES) {
    if (enemyName == *text) {
      return kind;
    }
  }
  throw UnknownEnemyException(
      engine::placeOf(name.source(), table, ENEMIES_KEY), *text, enemyNames());
}

/**
 * @returns The kinds the enemies of @p table name, in their order.
 * @throws MissingDataFileKeyException when it has none,
 * InvalidEnemyListException when they are not a list of ENEMY_LIST_LENGTH
 * names, and UnknownEnemyException for a name of no kind.
 */
std::vector<game::BydoKind> enemiesFrom(const DataFileTable& table) {
  const toml::node& node = engine::nodeAt(table, ENEMIES_KEY);
  const toml::array* const names = node.as_array();
  if (names == nullptr || !countFits(names->size(), ENEMY_LIST_LENGTH)) {
    throw InvalidEnemyListException(
        engine::placeOf(node.source(), table, ENEMIES_KEY), ENEMY_LIST_LENGTH);
  }
  std::vector<game::BydoKind> kinds;
  kinds.reserve(names->size());
  for (const toml::node& name : *names) {
    kinds.push_back(kindNamed(name, table));
  }
  return kinds;
}

/**
 * @returns How the waves of the table of one difficulty grow, every key
 * checked.
 */
game::WaveCurve curveFrom(const DataFileTable& table) {
  const std::int64_t firstIntervalMilliseconds = engine::wholeNumberAt(
      table, FIRST_SPAWN_INTERVAL_KEY, SPAWN_INTERVAL_RANGE);
  const engine::WholeNumberRange shortestIntervalRange{
      .lowest = SPAWN_INTERVAL_RANGE.lowest,
      .highest = firstIntervalMilliseconds};
  return game::WaveCurve{
      .firstWave =
          game::BydoWave{
              .bydoCount =
                  countAt(table, FIRST_WAVE_BYDOS_KEY, FIRST_WAVE_BYDOS_RANGE),
              .spawnInterval =
                  std::chrono::milliseconds{firstIntervalMilliseconds}},
      .bydosAddedPerWave =
          countAt(table, BYDOS_ADDED_PER_WAVE_KEY, BYDOS_ADDED_PER_WAVE_RANGE),
      .spawnIntervalCutPerWave = engine::millisecondsAt(
          table, SPAWN_INTERVAL_CUT_KEY, SPAWN_INTERVAL_CUT_RANGE),
      .shortestSpawnInterval = engine::millisecondsAt(
          table, SHORTEST_SPAWN_INTERVAL_KEY, shortestIntervalRange),
      .wavePause =
          engine::millisecondsAt(table, WAVE_PAUSE_KEY, WAVE_PAUSE_RANGE),
      .levelWaves = countAt(table, LEVEL_WAVES_KEY, LEVEL_WAVES_RANGE)};
}

/** @returns The waves the table of one difficulty describes. */
WavePlan planFrom(const DataFileTable& table) {
  engine::refuseUnknownKeys(table, WAVE_PLAN_KEYS);
  return WavePlan{.curve = curveFrom(table), .enemies = enemiesFrom(table)};
}

/** @returns The waves of every difficulty of the file whose root is @p root. */
PlansByDifficulty plansFrom(const DataFileTable& root) {
  engine::refuseUnknownKeys(root, ROOT_KEYS);
  const DataFileTable levels = engine::tableAt(root, DIFFICULTY_TABLE_KEY);
  PlansByDifficulty plans;
  for (auto&& entry : levels.table.get()) {
    const std::string_view name = entry.first.str();
    plans.emplace(std::string{name}, planFrom(engine::tableAt(levels, name)));
  }
  return plans;
}

}  // namespace

WaveCatalog WaveCatalog::fromText(std::string_view text,
                                  std::string_view source) {
  const toml::table root = engine::parseDataFile(text, source);
  return WaveCatalog{
      plansFrom(engine::rootOf(root, source)),
      network::sipHash(WAVES_FINGERPRINT_KEY, std::as_bytes(std::span{text}))};
}

WaveCatalog WaveCatalog::fromFile(const std::filesystem::path& file) {
  return fromText(engine::dataFileContent(file), engine::shownPath(file));
}

const WavePlan& WaveCatalog::planOf(std::string_view difficulty) const {
  const auto found = plans_.find(difficulty);
  if (found == plans_.end()) {
    throw UnknownDifficultyException(std::string{difficulty}, difficulties());
  }
  return found->second;
}

std::vector<std::string> WaveCatalog::difficulties() const {
  std::vector<std::string> names;
  names.reserve(plans_.size());
  for (const std::string& name : std::views::keys(plans_)) {
    names.push_back(name);
  }
  return names;
}

std::uint64_t WaveCatalog::fingerprint() const { return fingerprint_; }

WaveCatalog::WaveCatalog(PlansByDifficulty plans, std::uint64_t fingerprint)
    : plans_(std::move(plans)), fingerprint_(fingerprint) {}

}  // namespace rtype::server
