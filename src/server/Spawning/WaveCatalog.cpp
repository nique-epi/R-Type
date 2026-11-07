#include "WaveCatalog.hpp"
#include <algorithm>
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
#include "DataFileReading.hpp"
#include "DataFileTable.hpp"
#include "EntitySheet.hpp"
#include "EntitySheets.hpp"
#include "EntityType.hpp"
#include "ServerException.hpp"
#include "SipHash.hpp"
#include "Wave.hpp"
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

/** @returns The names of the sheets of @p types of the side enemy, in the
 * order of their names, for the errors. */
std::vector<std::string> enemyNamesOf(const game::EntitySheets& types) {
  std::vector<std::string> names;
  for (const game::EntitySheet& sheet : types.all()) {
    if (sheet.side == game::EntityType::Enemy) {
      names.push_back(sheet.name);
    }
  }
  return names;
}

/** @returns Whether @p types has a sheet named @p name of the side enemy. */
bool namesAnEnemy(const game::EntitySheets& types, std::string_view name) {
  return std::ranges::any_of(
      types.all(), [name](const game::EntitySheet& sheet) {
        return sheet.name == name && sheet.side == game::EntityType::Enemy;
      });
}

/** @brief The list of enemies of a table, and the sheets its names are checked
 * against. */
struct EnemyList {
  std::reference_wrapper<const DataFileTable> table;
  std::reference_wrapper<const game::EntitySheets> types;
};

/**
 * @returns The name @p name, an element of the enemies of the list, holds.
 * @throws InvalidEnemyListException when it is not a string, and
 * UnknownEnemyException when it names no sheet of the side enemy.
 */
std::string enemyNamed(const toml::node& name, const EnemyList& list) {
  const DataFileTable& table = list.table.get();
  const std::optional<std::string> text = name.value_exact<std::string>();
  if (!text.has_value()) {
    throw InvalidEnemyListException(
        engine::placeOf(name.source(), table, ENEMIES_KEY), ENEMY_LIST_LENGTH);
  }
  if (!namesAnEnemy(list.types.get(), *text)) {
    throw UnknownEnemyException(
        engine::placeOf(name.source(), table, ENEMIES_KEY), *text,
        enemyNamesOf(list.types.get()));
  }
  return *text;
}

/**
 * @returns The names the enemies of the list hold, in their order.
 * @throws MissingDataFileKeyException when the table has none,
 * InvalidEnemyListException when they are not a list of ENEMY_LIST_LENGTH
 * names, and UnknownEnemyException for a name of no enemy sheet.
 */
std::vector<std::string> enemiesFrom(const EnemyList& list) {
  const DataFileTable& table = list.table.get();
  const toml::node& node = engine::nodeAt(table, ENEMIES_KEY);
  const toml::array* const names = node.as_array();
  if (names == nullptr || !countFits(names->size(), ENEMY_LIST_LENGTH)) {
    throw InvalidEnemyListException(
        engine::placeOf(node.source(), table, ENEMIES_KEY), ENEMY_LIST_LENGTH);
  }
  std::vector<std::string> enemies;
  enemies.reserve(names->size());
  for (const toml::node& name : *names) {
    enemies.push_back(enemyNamed(name, list));
  }
  return enemies;
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
          game::Wave{.entityCount = countAt(table, FIRST_WAVE_BYDOS_KEY,
                                            FIRST_WAVE_BYDOS_RANGE),
                     .spawnInterval =
                         std::chrono::milliseconds{firstIntervalMilliseconds}},
      .entitiesAddedPerWave =
          countAt(table, BYDOS_ADDED_PER_WAVE_KEY, BYDOS_ADDED_PER_WAVE_RANGE),
      .spawnIntervalCutPerWave = engine::millisecondsAt(
          table, SPAWN_INTERVAL_CUT_KEY, SPAWN_INTERVAL_CUT_RANGE),
      .shortestSpawnInterval = engine::millisecondsAt(
          table, SHORTEST_SPAWN_INTERVAL_KEY, shortestIntervalRange),
      .wavePause =
          engine::millisecondsAt(table, WAVE_PAUSE_KEY, WAVE_PAUSE_RANGE),
      .levelWaves = countAt(table, LEVEL_WAVES_KEY, LEVEL_WAVES_RANGE)};
}

/** @returns The waves the table of one difficulty describes, its enemies
 * checked against @p types. */
/** @returns The names of the sheets of @p types that are bosses, for the
 * errors. */
std::vector<std::string> bossNamesOf(const game::EntitySheets& types) {
  std::vector<std::string> names;
  for (const game::EntitySheet& sheet : types.all()) {
    if (sheet.boss.has_value()) {
      names.push_back(sheet.name);
    }
  }
  return names;
}

/** @returns The boss the table of one difficulty names; nothing without one.
 * @throws engine::DataFileTextExpectedException when it is not a text, and
 * UnknownBossException when it names no boss of @p types. */
std::optional<std::string> bossFrom(const DataFileTable& table,
                                    const game::EntitySheets& types) {
  if (!table.table.get().contains(BOSS_KEY)) {
    return std::nullopt;
  }
  std::string boss = engine::textAt(table, BOSS_KEY);
  const std::vector<std::string> bosses = bossNamesOf(types);
  if (std::ranges::find(bosses, boss) == bosses.end()) {
    throw UnknownBossException(engine::placeOfKey(table, BOSS_KEY), boss,
                               bosses);
  }
  return boss;
}

WavePlan planFrom(const DataFileTable& table, const game::EntitySheets& types) {
  engine::refuseUnknownKeys(table, WAVE_PLAN_KEYS);
  return WavePlan{
      .curve = curveFrom(table),
      .enemies = enemiesFrom(EnemyList{.table = table, .types = types}),
      .boss = bossFrom(table, types)};
}

/** @returns The waves of every difficulty of the file whose root is @p root,
 * their enemies checked against @p types. */
PlansByDifficulty plansFrom(const DataFileTable& root,
                            const game::EntitySheets& types) {
  engine::refuseUnknownKeys(root, ROOT_KEYS);
  const DataFileTable levels = engine::tableAt(root, DIFFICULTY_TABLE_KEY);
  PlansByDifficulty plans;
  for (auto&& entry : levels.table.get()) {
    const std::string_view name = entry.first.str();
    plans.emplace(std::string{name},
                  planFrom(engine::tableAt(levels, name), types));
  }
  return plans;
}

}  // namespace

WaveCatalog WaveCatalog::fromText(std::string_view text,
                                  std::string_view source,
                                  const game::EntitySheets& types) {
  const toml::table root = engine::parseDataFile(text, source);
  return WaveCatalog{
      plansFrom(engine::rootOf(root, source), types),
      network::sipHash(WAVES_FINGERPRINT_KEY, std::as_bytes(std::span{text}))};
}

WaveCatalog WaveCatalog::fromFile(const std::filesystem::path& file,
                                  const game::EntitySheets& types) {
  return fromText(engine::dataFileContent(file), engine::shownPath(file),
                  types);
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
