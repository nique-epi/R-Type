#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <toml++/toml.hpp>
#include <vector>
#include "DataFilePlace.hpp"
#include "DataFileTable.hpp"
#include "TimeConstants.hpp"
#include "WholeNumberRange.hpp"

namespace rtype::engine {

/**
 * @brief Reads a data file, a TOML text, as strictly as the waves file: the
 * reader of a file asks each key it knows with the functions below, refuses
 * the others with refuseUnknownKeys(), and every error names the file, the
 * line and the column of what is wrong, and the dotted name of the key.
 *
 * @param source Names the file in the errors.
 * @returns The root table of @p text.
 * @throws DataFileSyntaxException when @p text is not TOML.
 */
[[nodiscard]] toml::table parseDataFile(std::string_view text,
                                        std::string_view source);

/**
 * @returns Everything @p file holds, read as bytes.
 * @throws UnreadableDataFileException, naming the file as shownPath() does,
 * when it cannot be opened.
 */
[[nodiscard]] std::string dataFileContent(const std::filesystem::path& file);

/** @returns How the errors show @p file: its absolute path, when the system
 * tells it, in the generic form. */
[[nodiscard]] std::string shownPath(const std::filesystem::path& file);

/**
 * @returns The root @p root of the file named @p source, whose name is empty.
 * Both must outlive the result: keep the table parseDataFile() returned in a
 * variable, never pass the call itself.
 */
[[nodiscard]] DataFileTable rootOf(const toml::table& root,
                                   std::string_view source);

/** @returns The dotted name of @p key of @p table: the key alone in the root,
 * `difficulty.normal.wave_pause_ms` in the table `difficulty.normal`. */
[[nodiscard]] std::string keyName(const DataFileTable& table,
                                  std::string_view key);

/** @returns Where @p region starts, about @p key of @p table. */
[[nodiscard]] DataFilePlace placeOf(const toml::source_region& region,
                                    const DataFileTable& table,
                                    std::string_view key);

/**
 * @returns Where the value of @p key of @p table starts, for an error about
 * that value.
 * @throws MissingDataFileKeyException, at the table, when it holds nothing.
 */
[[nodiscard]] DataFilePlace placeOfKey(const DataFileTable& table,
                                       std::string_view key);

/**
 * @throws UnknownDataFileKeyException for the first key of @p table that is
 * not one of @p knownKeys, at that key.
 */
void refuseUnknownKeys(const DataFileTable& table,
                       std::span<const std::string_view> knownKeys);

/**
 * @returns What @p key of @p table holds.
 * @throws MissingDataFileKeyException, at the table, when it holds nothing.
 */
[[nodiscard]] const toml::node& nodeAt(const DataFileTable& table,
                                       std::string_view key);

/**
 * @returns The table @p key of @p parent holds, named after its dotted name.
 * @throws MissingDataFileKeyException when it holds nothing, and
 * DataFileTableExpectedException when it holds something else.
 */
[[nodiscard]] DataFileTable tableAt(const DataFileTable& parent,
                                    std::string_view key);

/**
 * @returns The table @p key of @p parent holds, as tableAt() does, or nothing
 * when @p parent has no key @p key: a table the file may leave out.
 * @throws DataFileTableExpectedException when the key holds something else
 * than a table.
 */
[[nodiscard]] std::optional<DataFileTable> optionalTableAt(
    const DataFileTable& parent, std::string_view key);

/**
 * @returns The tables of the array @p key of @p parent holds, in the order of
 * the file (`[[gameplay.hurt_zone.box]]`), each named after its index:
 * `gameplay.hurt_zone.box[0]`. An empty array gives no table.
 * @throws MissingDataFileKeyException when it holds nothing, and
 * DataFileTablesExpectedException when it holds anything but an array of
 * tables.
 */
[[nodiscard]] std::vector<DataFileTable> tablesAt(const DataFileTable& parent,
                                                  std::string_view key);

/**
 * @returns The integer @p key of @p table holds.
 * @throws MissingDataFileKeyException when it holds nothing, and
 * DataFileValueOutOfRangeException, at the value, when it holds anything but
 * a TOML integer of @p range, a float such as 2500.0 included.
 */
[[nodiscard]] std::int64_t wholeNumberAt(const DataFileTable& table,
                                         std::string_view key,
                                         WholeNumberRange range);

/**
 * @returns The duration @p key of @p table holds, a whole number of
 * milliseconds within @p range (the key says it in its name, `_ms`).
 * @throws What wholeNumberAt() throws.
 */
[[nodiscard]] Duration millisecondsAt(const DataFileTable& table,
                                      std::string_view key,
                                      WholeNumberRange range);

/**
 * @returns The text @p key of @p table holds.
 * @throws MissingDataFileKeyException when it holds nothing, and
 * DataFileTextExpectedException, at the value, when it holds anything but a
 * TOML string.
 */
[[nodiscard]] std::string textAt(const DataFileTable& table,
                                 std::string_view key);

/**
 * @returns The position in @p choices of the text @p key of @p table holds:
 * 0 for the first choice.
 * @throws What textAt() throws, and UnknownDataFileChoiceException, at the
 * value, when the text is none of @p choices; the message lists them.
 */
[[nodiscard]] std::size_t choiceAt(const DataFileTable& table,
                                   std::string_view key,
                                   std::span<const std::string_view> choices);

/**
 * @returns The regular files of @p folder whose extension is
 * DATA_FILE_EXTENSION, sorted by path, so by name: the order is the same on
 * every system. The sub-folders are not read.
 * @throws UnreadableDataFileException, naming the folder as shownPath() does,
 * when it cannot be listed.
 */
[[nodiscard]] std::vector<std::filesystem::path> dataFilesIn(
    const std::filesystem::path& folder);

}  // namespace rtype::engine
