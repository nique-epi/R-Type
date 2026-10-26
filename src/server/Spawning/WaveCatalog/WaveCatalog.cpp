#include "WaveCatalog.hpp"
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <functional>
#include <ios>
#include <map>
#include <optional>
#include <ranges>
#include <span>
#include <sstream>
#include <string>
#include <string_view>
#include <system_error>
#include <toml++/toml.hpp>
#include <utility>
#include <vector>
#include "BydoKind.hpp"
#include "BydoWave.hpp"
#include "ServerException.hpp"
#include "TimeConstants.hpp"
#include "WaveCurve.hpp"
#include "WaveFileConstants.hpp"
#include "WaveFilePlace.hpp"
#include "WavePlan.hpp"
#include "WholeNumberRange.hpp"

namespace rtype::server {

namespace {

using PlansByDifficulty = std::map<std::string, WavePlan, std::less<>>;

/**
 * @brief A table of a waves file: the table, the file it was read from, and
 * its dotted name (`difficulty.normal`), empty for the root of the file.
 */
struct FileTable {
  std::reference_wrapper<const toml::table> table;
  std::string_view source;
  std::string name;
};

/** @returns The dotted name of @p key of @p table. */
std::string keyName(const FileTable& table, std::string_view key) {
  if (table.name.empty()) {
    return std::string{key};
  }
  return table.name + WAVE_FILE_KEY_SEPARATOR + std::string{key};
}

/** @returns Where @p region starts, about @p key of @p table. */
WaveFilePlace placeOf(const toml::source_region& region, const FileTable& table,
                      std::string_view key) {
  return WaveFilePlace{.file = std::string{table.source},
                       .line = region.begin.line,
                       .column = region.begin.column,
                       .key = keyName(table, key)};
}

/** @returns Whether @p count is within @p range. */
bool countFits(std::size_t count, WholeNumberRange range) {
  const auto signedCount = static_cast<std::int64_t>(count);
  return signedCount >= range.lowest && signedCount <= range.highest;
}

/**
 * @throws UnknownWaveFileKeyException for the first key of @p table that is
 * not one of @p knownKeys.
 */
void refuseUnknownKeys(const FileTable& table,
                       std::span<const std::string_view> knownKeys) {
  for (auto&& entry : table.table.get()) {
    const toml::key& key = entry.first;
    if (std::ranges::find(knownKeys, key.str()) == knownKeys.end()) {
      throw UnknownWaveFileKeyException(
          placeOf(key.source(), table, key.str()));
    }
  }
}

/**
 * @returns What @p key of @p table holds.
 * @throws MissingWaveFileKeyException when it holds nothing.
 */
const toml::node& nodeAt(const FileTable& table, std::string_view key) {
  const toml::node* const node = table.table.get().get(key);
  if (node == nullptr) {
    throw MissingWaveFileKeyException(
        placeOf(table.table.get().source(), table, key));
  }
  return *node;
}

/**
 * @returns The table @p key of @p parent holds.
 * @throws MissingWaveFileKeyException when it holds nothing, and
 * WaveFileTableExpectedException when it holds something else.
 */
const toml::table& tableAt(const FileTable& parent, std::string_view key) {
  const toml::node& node = nodeAt(parent, key);
  const toml::table* const table = node.as_table();
  if (table == nullptr) {
    throw WaveFileTableExpectedException(placeOf(node.source(), parent, key));
  }
  return *table;
}

/**
 * @returns The integer @p key of @p table holds.
 * @throws MissingWaveFileKeyException when it holds nothing, and
 * WaveFileValueOutOfRangeException when it holds anything but a TOML integer
 * of @p range, a float such as 2500.0 included.
 */
std::int64_t wholeNumberAt(const FileTable& table, std::string_view key,
                           WholeNumberRange range) {
  const toml::node& node = nodeAt(table, key);
  const std::optional<std::int64_t> number = node.value_exact<std::int64_t>();
  if (!number.has_value() || *number < range.lowest ||
      *number > range.highest) {
    throw WaveFileValueOutOfRangeException(placeOf(node.source(), table, key),
                                           range);
  }
  return *number;
}

/** @returns The count @p key of @p table holds; throws as wholeNumberAt(). */
std::size_t countAt(const FileTable& table, std::string_view key,
                    WholeNumberRange range) {
  return static_cast<std::size_t>(wholeNumberAt(table, key, range));
}

/**
 * @returns The milliseconds @p key of @p table holds; throws as
 * wholeNumberAt().
 */
engine::Duration millisecondsAt(const FileTable& table, std::string_view key,
                                WholeNumberRange range) {
  return std::chrono::milliseconds{wholeNumberAt(table, key, range)};
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
game::BydoKind kindNamed(const toml::node& name, const FileTable& table) {
  const std::optional<std::string> text = name.value_exact<std::string>();
  if (!text.has_value()) {
    throw InvalidEnemyListException(placeOf(name.source(), table, ENEMIES_KEY),
                                    ENEMY_LIST_LENGTH);
  }
  for (const auto& [enemyName, kind] : ENEMY_NAMES) {
    if (enemyName == *text) {
      return kind;
    }
  }
  throw UnknownEnemyException(placeOf(name.source(), table, ENEMIES_KEY), *text,
                              enemyNames());
}

/**
 * @returns The kinds the enemies of @p table name, in their order.
 * @throws MissingWaveFileKeyException when it has none,
 * InvalidEnemyListException when they are not a list of ENEMY_LIST_LENGTH
 * names, and UnknownEnemyException for a name of no kind.
 */
std::vector<game::BydoKind> enemiesFrom(const FileTable& table) {
  const toml::node& node = nodeAt(table, ENEMIES_KEY);
  const toml::array* const names = node.as_array();
  if (names == nullptr || !countFits(names->size(), ENEMY_LIST_LENGTH)) {
    throw InvalidEnemyListException(placeOf(node.source(), table, ENEMIES_KEY),
                                    ENEMY_LIST_LENGTH);
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
game::WaveCurve curveFrom(const FileTable& table) {
  const std::int64_t firstIntervalMilliseconds =
      wholeNumberAt(table, FIRST_SPAWN_INTERVAL_KEY, SPAWN_INTERVAL_RANGE);
  const WholeNumberRange shortestIntervalRange{
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
      .spawnIntervalCutPerWave = millisecondsAt(table, SPAWN_INTERVAL_CUT_KEY,
                                                SPAWN_INTERVAL_CUT_RANGE),
      .shortestSpawnInterval = millisecondsAt(
          table, SHORTEST_SPAWN_INTERVAL_KEY, shortestIntervalRange),
      .wavePause = millisecondsAt(table, WAVE_PAUSE_KEY, WAVE_PAUSE_RANGE)};
}

/** @returns The waves the table of one difficulty describes. */
WavePlan planFrom(const FileTable& table) {
  refuseUnknownKeys(table, WAVE_PLAN_KEYS);
  return WavePlan{.curve = curveFrom(table), .enemies = enemiesFrom(table)};
}

/** @returns The waves of every difficulty of the file whose root is @p root. */
PlansByDifficulty plansFrom(const FileTable& root) {
  refuseUnknownKeys(root, ROOT_KEYS);
  const FileTable levels{.table = tableAt(root, DIFFICULTY_TABLE_KEY),
                         .source = root.source,
                         .name = keyName(root, DIFFICULTY_TABLE_KEY)};
  PlansByDifficulty plans;
  for (auto&& entry : levels.table.get()) {
    const std::string_view name = entry.first.str();
    const FileTable level{.table = tableAt(levels, name),
                          .source = root.source,
                          .name = keyName(levels, name)};
    plans.emplace(std::string{name}, planFrom(level));
  }
  return plans;
}

/**
 * @returns How the errors show @p file: its absolute path, when the system
 * tells it.
 */
std::string shownPath(const std::filesystem::path& file) {
  std::error_code error;
  const std::filesystem::path absolute = std::filesystem::absolute(file, error);
  return (error ? file : absolute).generic_string();
}

/**
 * @returns Everything @p file holds.
 * @throws UnreadableWaveFileException, naming it @p shown, when it cannot be
 * opened.
 */
std::string contentOf(const std::filesystem::path& file,
                      const std::string& shown) {
  const std::ifstream stream{file, std::ios::binary};
  if (!stream.is_open()) {
    throw UnreadableWaveFileException(shown);
  }
  std::ostringstream content;
  content << stream.rdbuf();
  return content.str();
}

}  // namespace

WaveCatalog WaveCatalog::fromText(std::string_view text,
                                  std::string_view source) {
  toml::table root;
  try {
    root = toml::parse(text, source);
  } catch (const toml::parse_error& error) {
    const toml::source_region& region = error.source();
    throw WaveFileSyntaxException(WaveFilePlace{.file = std::string{source},
                                                .line = region.begin.line,
                                                .column = region.begin.column,
                                                .key = {}},
                                  std::string{error.description()});
  }
  return WaveCatalog{
      plansFrom(FileTable{.table = root, .source = source, .name = {}})};
}

WaveCatalog WaveCatalog::fromFile(const std::filesystem::path& file) {
  const std::string shown = shownPath(file);
  return fromText(contentOf(file, shown), shown);
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

WaveCatalog::WaveCatalog(PlansByDifficulty plans) : plans_(std::move(plans)) {}

}  // namespace rtype::server
