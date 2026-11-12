#include "WaveCatalog.hpp"
#include <algorithm>
#include <array>
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
#include "DifficultyLayers.hpp"
#include "EntitySheet.hpp"
#include "EntitySheets.hpp"
#include "EntityType.hpp"
#include "GameRuleConstants.hpp"
#include "ServerException.hpp"
#include "SipHash.hpp"
#include "TimeConstants.hpp"
#include "Wave.hpp"
#include "WaveCurve.hpp"
#include "WaveFileConstants.hpp"
#include "WaveFileText.hpp"
#include "WavePlan.hpp"
#include "WholeNumberRange.hpp"

namespace rtype::server {

namespace {

using PlansByDifficulty = std::map<std::string, WavePlan, std::less<>>;
using engine::DataFileTable;

/** @returns Where @p name stands in DIFFICULTY_ORDER; the size of that order
 * for a difficulty it does not name, so it comes after every one it names. */
std::size_t rankOf(std::string_view name) {
  return static_cast<std::size_t>(
      std::ranges::find(identifiers::DIFFICULTY_ORDER, name) -
      identifiers::DIFFICULTY_ORDER.begin());
}

/** @returns Whether @p count is within @p range. */
bool countFits(std::size_t count, engine::WholeNumberRange range) {
  const auto signedCount = static_cast<std::int64_t>(count);
  return signedCount >= range.lowest && signedCount <= range.highest;
}

/** @returns The integer @p key holds in the layer of @p layers that gives it;
 * throws as engine::wholeNumberAt(). */
std::int64_t wholeNumberOf(const DifficultyLayers& layers, std::string_view key,
                           engine::WholeNumberRange range) {
  return engine::wholeNumberAt(layerHolding(layers, key), key, range);
}

/** @returns The count @p key holds in the layer of @p layers that gives it;
 * throws as engine::wholeNumberAt(). */
std::size_t countOf(const DifficultyLayers& layers, std::string_view key,
                    engine::WholeNumberRange range) {
  return static_cast<std::size_t>(wholeNumberOf(layers, key, range));
}

/** @returns The duration @p key holds in the layer of @p layers that gives
 * it; throws as engine::millisecondsAt(). */
engine::Duration millisecondsOf(const DifficultyLayers& layers,
                                std::string_view key,
                                engine::WholeNumberRange range) {
  return engine::millisecondsAt(layerHolding(layers, key), key, range);
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
      types.all(), [name](const game::EntitySheet& sheet) noexcept {
        return sheet.name == name && sheet.side == game::EntityType::Enemy;
      });
}

/** @returns The names of the sheets of @p types of the side look_item, in the
 * order of their names, for the errors. */
std::vector<std::string> lookItemNamesOf(const game::EntitySheets& types) {
  std::vector<std::string> names;
  for (const game::EntitySheet& sheet : types.all()) {
    if (sheet.side == game::EntityType::LookItem) {
      names.push_back(sheet.name);
    }
  }
  return names;
}

/** @returns Whether @p types has a sheet named @p name of the side look_item.
 */
bool namesALookItem(const game::EntitySheets& types, std::string_view name) {
  return std::ranges::any_of(
      types.all(), [name](const game::EntitySheet& sheet) {
        return sheet.name == name && sheet.side == game::EntityType::LookItem;
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
 * @returns The name @p name, an element of the look items of the table,
 * holds.
 * @throws InvalidLookItemListException when it is not a string, and
 * UnknownLookItemException when it names no sheet of the side look_item.
 */
std::string lookItemNamed(const toml::node& name, const EnemyList& list) {
  const DataFileTable& table = list.table.get();
  const std::optional<std::string> text = name.value_exact<std::string>();
  if (!text.has_value()) {
    throw InvalidLookItemListException(
        engine::placeOf(name.source(), table, LOOK_ITEMS_KEY),
        LOOK_ITEM_LIST_LENGTH);
  }
  if (!namesALookItem(list.types.get(), *text)) {
    throw UnknownLookItemException(
        engine::placeOf(name.source(), table, LOOK_ITEMS_KEY), *text,
        lookItemNamesOf(list.types.get()));
  }
  return *text;
}

/**
 * @returns The names the look items of the list hold, in their order; none
 * when the table has no such key.
 * @throws InvalidLookItemListException when they are not a list of
 * LOOK_ITEM_LIST_LENGTH names, and UnknownLookItemException for a name of no
 * look item sheet.
 */
std::vector<std::string> lookItemsFrom(const EnemyList& list) {
  const DataFileTable& table = list.table.get();
  if (!table.table.get().contains(LOOK_ITEMS_KEY)) {
    return {};
  }
  const toml::node& node = engine::nodeAt(table, LOOK_ITEMS_KEY);
  const toml::array* const names = node.as_array();
  if (names == nullptr || !countFits(names->size(), LOOK_ITEM_LIST_LENGTH)) {
    throw InvalidLookItemListException(
        engine::placeOf(node.source(), table, LOOK_ITEMS_KEY),
        LOOK_ITEM_LIST_LENGTH);
  }
  std::vector<std::string> items;
  items.reserve(names->size());
  for (const toml::node& name : *names) {
    items.push_back(lookItemNamed(name, list));
  }
  return items;
}

/**
 * @returns How the waves of one difficulty grow, every key read from the
 * layer of @p layers that gives it and checked.
 */
game::WaveCurve curveFrom(const DifficultyLayers& layers) {
  const std::int64_t firstIntervalMilliseconds =
      wholeNumberOf(layers, FIRST_SPAWN_INTERVAL_KEY, SPAWN_INTERVAL_RANGE);
  const engine::WholeNumberRange shortestIntervalRange{
      .lowest = SPAWN_INTERVAL_RANGE.lowest,
      .highest = firstIntervalMilliseconds};
  return game::WaveCurve{
      .firstWave =
          game::Wave{.entityCount = countOf(layers, FIRST_WAVE_BYDOS_KEY,
                                            FIRST_WAVE_BYDOS_RANGE),
                     .spawnInterval =
                         std::chrono::milliseconds{firstIntervalMilliseconds}},
      .entitiesAddedPerWave =
          countOf(layers, BYDOS_ADDED_PER_WAVE_KEY, BYDOS_ADDED_PER_WAVE_RANGE),
      .spawnIntervalCutPerWave = millisecondsOf(layers, SPAWN_INTERVAL_CUT_KEY,
                                                SPAWN_INTERVAL_CUT_RANGE),
      .shortestSpawnInterval = millisecondsOf(
          layers, SHORTEST_SPAWN_INTERVAL_KEY, shortestIntervalRange),
      .wavePause = millisecondsOf(layers, WAVE_PAUSE_KEY, WAVE_PAUSE_RANGE),
      .levelWaves = countOf(layers, LEVEL_WAVES_KEY, LEVEL_WAVES_RANGE)};
}

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

/** @returns The health percent of the boss the table of one difficulty
 * gives, DEFAULT_BOSS_HEALTH_PERCENT when it gives none. */
int bossHealthPercentFrom(const DataFileTable& table) {
  if (!table.table.get().contains(BOSS_HEALTH_PERCENT_KEY)) {
    return DEFAULT_BOSS_HEALTH_PERCENT;
  }
  return static_cast<int>(engine::wholeNumberAt(table, BOSS_HEALTH_PERCENT_KEY,
                                                BOSS_HEALTH_PERCENT_RANGE));
}

/** @returns The waves one difficulty describes, each key read from the
 * layer of @p layers that gives it, its enemies checked against @p types. */
WavePlan planFrom(const DifficultyLayers& layers,
                  const game::EntitySheets& types) {
  return WavePlan{
      .curve = curveFrom(layers),
      .enemies = enemiesFrom(EnemyList{
          .table = layerHolding(layers, ENEMIES_KEY), .types = types}),
      .boss = bossFrom(layerHolding(layers, BOSS_KEY), types),
      .bossHealthPercent =
          bossHealthPercentFrom(layerHolding(layers, BOSS_HEALTH_PERCENT_KEY)),
      .lookItems = lookItemsFrom(EnemyList{
          .table = layerHolding(layers, LOOK_ITEMS_KEY), .types = types})};
}

/** @returns The waves of every difficulty of @p laid, their enemies checked
 * against @p types. */
PlansByDifficulty plansFrom(const LayersByDifficulty& laid,
                            const game::EntitySheets& types) {
  PlansByDifficulty plans;
  for (const auto& [name, layers] : laid) {
    plans.emplace(name, planFrom(layers, types));
  }
  return plans;
}

/** @returns The root table of each of @p files, in their order.
 * @throws engine::DataFileSyntaxException, naming the file at fault, when one
 * is not TOML. */
std::vector<toml::table> rootsOf(std::span<const WaveFileText> files) {
  std::vector<toml::table> roots;
  roots.reserve(files.size());
  for (const WaveFileText& file : files) {
    roots.push_back(engine::parseDataFile(file.text, file.source));
  }
  return roots;
}

/** @returns SipHash-2-4 of the texts of @p files joined by
 * WAVE_FILE_SEPARATOR, under WAVES_FINGERPRINT_KEY. */
std::uint64_t fingerprintOf(std::span<const WaveFileText> files) {
  std::string laid;
  for (std::size_t index = 0; index < files.size(); ++index) {
    if (index > 0) {
      laid += WAVE_FILE_SEPARATOR;
    }
    laid += files[index].text;
  }
  return network::sipHash(WAVES_FINGERPRINT_KEY,
                          std::as_bytes(std::span{laid}));
}

}  // namespace

WaveCatalog WaveCatalog::fromText(std::string_view text,
                                  std::string_view source,
                                  const game::EntitySheets& types) {
  const std::array files{
      WaveFileText{.text = std::string{text}, .source = std::string{source}}};
  return fromTexts(files, types);
}

WaveCatalog WaveCatalog::fromTexts(std::span<const WaveFileText> files,
                                   const game::EntitySheets& types) {
  const std::vector<toml::table> roots = rootsOf(files);
  std::vector<DataFileTable> layers;
  layers.reserve(roots.size());
  for (std::size_t index = 0; index < roots.size(); ++index) {
    layers.push_back(engine::rootOf(roots[index], files[index].source));
  }
  return WaveCatalog{plansFrom(difficultyLayersOf(layers), types),
                     fingerprintOf(files)};
}

WaveCatalog WaveCatalog::fromFiles(std::span<const std::filesystem::path> files,
                                   const game::EntitySheets& types) {
  std::vector<WaveFileText> texts;
  texts.reserve(files.size());
  for (const std::filesystem::path& file : files) {
    texts.push_back(WaveFileText{.text = engine::dataFileContent(file),
                                 .source = engine::shownPath(file)});
  }
  return fromTexts(texts, types);
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
  std::ranges::stable_sort(names, std::less<>{}, rankOf);
  return names;
}

std::uint64_t WaveCatalog::fingerprint() const { return fingerprint_; }

WaveCatalog::WaveCatalog(PlansByDifficulty plans, std::uint64_t fingerprint)
    : plans_(std::move(plans)), fingerprint_(fingerprint) {}

}  // namespace rtype::server
