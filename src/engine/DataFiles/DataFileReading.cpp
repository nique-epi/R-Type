#include "DataFileReading.hpp"
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <ios>
#include <optional>
#include <span>
#include <sstream>
#include <string>
#include <string_view>
#include <system_error>
#include <toml++/toml.hpp>
#include <vector>
#include "DataFileConstants.hpp"
#include "DataFilePlace.hpp"
#include "DataFileTable.hpp"
#include "EngineException.hpp"
#include "PathText.hpp"
#include "TimeConstants.hpp"
#include "WholeNumberRange.hpp"

namespace rtype::engine {

namespace {

/** @returns Whether @p entry is a regular file whose extension is
 * DATA_FILE_EXTENSION. */
bool isDataFile(const std::filesystem::directory_entry& entry) {
  std::error_code ignored;
  return entry.is_regular_file(ignored) &&
         entry.path().extension() == std::filesystem::path{DATA_FILE_EXTENSION};
}

/** @returns The name of the table at @p index of the array @p key of
 * @p parent: `gameplay.hurt_zone.box[0]`. */
std::string elementName(const DataFileTable& parent, std::string_view key,
                        std::size_t index) {
  return keyName(parent, key) + DATA_FILE_INDEX_OPENING +
         std::to_string(index) + DATA_FILE_INDEX_CLOSING;
}

}  // namespace

toml::table parseDataFile(std::string_view text, std::string_view source) {
  toml::table root;
  try {
    root = toml::parse(text, source);
  } catch (const toml::parse_error& error) {
    const toml::source_region& region = error.source();
    throw DataFileSyntaxException(DataFilePlace{.file = std::string{source},
                                                .line = region.begin.line,
                                                .column = region.begin.column,
                                                .key = {}},
                                  std::string{error.description()});
  }
  return root;
}

std::string dataFileContent(const std::filesystem::path& file) {
  const std::ifstream stream{file, std::ios::binary};
  if (!stream.is_open()) {
    throw UnreadableDataFileException(shownPath(file));
  }
  std::ostringstream content;
  content << stream.rdbuf();
  return content.str();
}

std::string shownPath(const std::filesystem::path& file) {
  std::error_code error;
  const std::filesystem::path absolute = std::filesystem::absolute(file, error);
  return logging::genericText(error ? file : absolute);
}

DataFileTable rootOf(const toml::table& root, std::string_view source) {
  return DataFileTable{.table = root, .source = source, .name = {}};
}

std::string keyName(const DataFileTable& table, std::string_view key) {
  if (table.name.empty()) {
    return std::string{key};
  }
  return table.name + DATA_FILE_KEY_SEPARATOR + std::string{key};
}

DataFilePlace placeOf(const toml::source_region& region,
                      const DataFileTable& table, std::string_view key) {
  return DataFilePlace{.file = std::string{table.source},
                       .line = region.begin.line,
                       .column = region.begin.column,
                       .key = keyName(table, key)};
}

DataFilePlace placeOfKey(const DataFileTable& table, std::string_view key) {
  return placeOf(nodeAt(table, key).source(), table, key);
}

void refuseUnknownKeys(const DataFileTable& table,
                       std::span<const std::string_view> knownKeys) {
  for (auto&& entry : table.table.get()) {
    const toml::key& key = entry.first;
    if (std::ranges::find(knownKeys, key.str()) == knownKeys.end()) {
      throw UnknownDataFileKeyException(
          placeOf(key.source(), table, key.str()));
    }
  }
}

const toml::node& nodeAt(const DataFileTable& table, std::string_view key) {
  const toml::node* const node = table.table.get().get(key);
  if (node == nullptr) {
    throw MissingDataFileKeyException(
        placeOf(table.table.get().source(), table, key));
  }
  return *node;
}

DataFileTable tableAt(const DataFileTable& parent, std::string_view key) {
  const toml::node& node = nodeAt(parent, key);
  const toml::table* const table = node.as_table();
  if (table == nullptr) {
    throw DataFileTableExpectedException(placeOf(node.source(), parent, key));
  }
  return DataFileTable{
      .table = *table, .source = parent.source, .name = keyName(parent, key)};
}

std::optional<DataFileTable> optionalTableAt(const DataFileTable& parent,
                                             std::string_view key) {
  if (parent.table.get().get(key) == nullptr) {
    return std::nullopt;
  }
  return tableAt(parent, key);
}

std::vector<DataFileTable> tablesAt(const DataFileTable& parent,
                                    std::string_view key) {
  const toml::node& node = nodeAt(parent, key);
  const toml::array* const array = node.as_array();
  if (array == nullptr) {
    throw DataFileTablesExpectedException(placeOf(node.source(), parent, key));
  }
  std::vector<DataFileTable> tables;
  for (std::size_t index = 0; index < array->size(); ++index) {
    const toml::table* const element = array->get(index)->as_table();
    if (element == nullptr) {
      throw DataFileTablesExpectedException(
          placeOf(array->get(index)->source(), parent, key));
    }
    tables.push_back(DataFileTable{.table = *element,
                                   .source = parent.source,
                                   .name = elementName(parent, key, index)});
  }
  return tables;
}

std::int64_t wholeNumberAt(const DataFileTable& table, std::string_view key,
                           WholeNumberRange range) {
  const toml::node& node = nodeAt(table, key);
  const std::optional<std::int64_t> number = node.value_exact<std::int64_t>();
  if (!number.has_value() || *number < range.lowest ||
      *number > range.highest) {
    throw DataFileValueOutOfRangeException(placeOf(node.source(), table, key),
                                           range);
  }
  return *number;
}

Duration millisecondsAt(const DataFileTable& table, std::string_view key,
                        WholeNumberRange range) {
  return std::chrono::milliseconds{wholeNumberAt(table, key, range)};
}

std::string textAt(const DataFileTable& table, std::string_view key) {
  const toml::node& node = nodeAt(table, key);
  const std::optional<std::string> text = node.value_exact<std::string>();
  if (!text.has_value()) {
    throw DataFileTextExpectedException(placeOf(node.source(), table, key));
  }
  return *text;
}

std::size_t choiceAt(const DataFileTable& table, std::string_view key,
                     std::span<const std::string_view> choices) {
  const std::string text = textAt(table, key);
  const auto found = std::ranges::find(choices, text);
  if (found == choices.end()) {
    throw UnknownDataFileChoiceException(placeOfKey(table, key), text, choices);
  }
  return static_cast<std::size_t>(found - choices.begin());
}

std::vector<std::filesystem::path> dataFilesIn(
    const std::filesystem::path& folder) {
  std::error_code error;
  std::vector<std::filesystem::path> files;
  for (std::filesystem::directory_iterator entry{folder, error};
       !error && entry != std::filesystem::directory_iterator{};
       entry.increment(error)) {
    if (isDataFile(*entry)) {
      files.push_back(entry->path());
    }
  }
  if (error) {
    throw UnreadableDataFileException(shownPath(folder));
  }
  std::ranges::sort(files);
  return files;
}

}  // namespace rtype::engine
