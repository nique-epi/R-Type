#pragma once

#include <filesystem>
#include <functional>
#include <map>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include "EntityAppearance.hpp"
#include "SheetText.hpp"

namespace rtype::client {

/**
 * @brief The appearance of every entity type the client can draw, read from
 * the `[appearance]` table of each entity sheet, found by the name of its
 * sheet.
 *
 * The reading is as strict as the server's: the root of a sheet holds exactly
 * `[gameplay]` and `[appearance]`, `[appearance]` holds `sheet` and the table
 * `idle`, and `idle` holds every key of IDLE_KEYS, each an integer in its
 * range; `[appearance]` may also hold `attack_<n>` tables, each with a table
 * per phase named like the phases of the server (`signal`, `strike`,
 * `recover`, `enraged`), whose keys are ACTION_KEYS; a missing key, an unknown
 * key or a value out of range throws, naming the file, the line, the column and
 * the dotted key. `[gameplay]` belongs to the server: only its presence is
 * checked, never its keys. Whether the sprite sheet an appearance names exists
 * is checked when the game loads it.
 *
 * The appearances never move once read, so an Animation may view their
 * frames for as long as this object lives.
 */
class EntityAppearances {
 public:
  /**
   * @brief Reads every `.toml` file of @p folder, each named after its type,
   * as game::sheetTextsIn() lists them.
   * @throws engine::UnreadableDataFileException when the folder or a file
   * cannot be read, and what fromTexts() throws, naming the file by its
   * absolute path.
   */
  [[nodiscard]] static EntityAppearances fromFolder(
      const std::filesystem::path& folder);

  /**
   * @brief Reads the sheets @p sheets hold, each named by its name; the
   * errors name the file by its source.
   * @throws engine::DataFileSyntaxException, MissingDataFileKeyException,
   * UnknownDataFileKeyException, DataFileTableExpectedException,
   * DataFileTextExpectedException or DataFileValueOutOfRangeException when a
   * sheet is not as described above.
   */
  [[nodiscard]] static EntityAppearances fromTexts(
      std::span<const game::SheetText> sheets);

  /** @returns The appearance of the type @p name, or nullptr when no sheet
   * of that name was read. */
  [[nodiscard]] const EntityAppearance* named(std::string_view name) const;

  /** @returns The asset id of every sprite sheet an appearance names, each
   * once, in alphabetical order. */
  [[nodiscard]] std::vector<std::string> sheets() const;

 private:
  explicit EntityAppearances(
      std::map<std::string, EntityAppearance, std::less<>> appearances);

  std::map<std::string, EntityAppearance, std::less<>> appearances_;
};

}  // namespace rtype::client
