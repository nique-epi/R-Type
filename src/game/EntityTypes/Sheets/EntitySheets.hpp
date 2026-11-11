#pragma once

#include <cstdint>
#include <filesystem>
#include <span>
#include <string_view>
#include <vector>
#include "BehaviourRegistry.hpp"
#include "EntitySheet.hpp"
#include "SheetText.hpp"

namespace rtype::game {

/**
 * @brief Every entity type a program knows, one sheet each, read once when it
 * starts, in the order of their names and numbered from 1 in that order.
 *
 * It owns the behaviours the pilots of its sheets name (builtInBehaviours()).
 * It can be moved, never copied, and a reference into it survives a move:
 * BehaviourRegistry keeps each behaviour behind its own std::unique_ptr, so
 * moving the registry moves no behaviour; moving a std::vector hands its
 * buffer over, so the sheets, and the std::vector of the settings of each
 * pilot, stay where they are. A Pilot made from a sheet may therefore keep its
 * behaviour and the span of its settings for as long as the catalog lives,
 * wherever it was moved.
 * Every name is 1 to MAXIMUM_CONTENT_NAME_LENGTH lowercase letters, digits
 * and underscores, at most MAXIMUM_ENTITY_TYPES sheets are read, no two share a
 * name, each is read by gameplayOf(), and a gun fires a sheet that exists, of
 * the side of a missile of its shooter, whose number its GunSheet then holds.
 */
class EntitySheets {
 public:
  EntitySheets(const EntitySheets&) = delete;
  EntitySheets& operator=(const EntitySheets&) = delete;
  EntitySheets(EntitySheets&&) noexcept = default;
  EntitySheets& operator=(EntitySheets&&) noexcept = default;
  ~EntitySheets() = default;

  /**
   * @brief Reads the sheets sheetTextsIn() finds in @p folder. It never gives
   * an empty catalog: a program with no type could draw nothing.
   * @throws What sheetTextsIn() throws, and what fromTexts() throws.
   */
  [[nodiscard]] static EntitySheets fromFolder(
      const std::filesystem::path& folder);

  /**
   * @brief Reads the sheets of @p texts, in any order.
   * @throws What checkSheetNames() throws, DuplicateEntityTypeException when
   * two share a name; what gameplayOf()
   * throws for a sheet; engine::UnknownDataFileChoiceException when a gun fires
   * no sheet, and InconsistentEntitySheetException when it fires a sheet of
   * the wrong side.
   */
  [[nodiscard]] static EntitySheets fromTexts(std::span<const SheetText> texts);

  /**
   * @brief Reads the sheets of @p texts as the other fromTexts() does, their
   * pilots naming the behaviours of @p behaviours (the built-in ones and those
   * the server scripts registered, see BehaviourScripts), which it keeps.
   * @throws What the other fromTexts() throws.
   */
  [[nodiscard]] static EntitySheets fromTexts(std::span<const SheetText> texts,
                                              BehaviourRegistry behaviours);

  /**
   * @returns The sheet named @p name.
   * @throws UnknownEntityTypeException when none is; the message lists them.
   */
  [[nodiscard]] const EntitySheet& named(std::string_view name) const;

  /** @returns The sheet numbered @p number, nullptr for 0 or a number past the
   * last sheet. */
  [[nodiscard]] const EntitySheet* numbered(std::uint8_t number) const;

  /** @returns Every sheet, in the order of their names, so of their numbers.
   */
  [[nodiscard]] std::span<const EntitySheet> all() const;

  /** @returns The texts the sheets were read from, in the order of their
   * names: the server hashes them to tell whether its games are vanilla. */
  [[nodiscard]] std::span<const SheetText> texts() const;

 private:
  EntitySheets(std::vector<SheetText> texts, BehaviourRegistry behaviours,
               std::vector<EntitySheet> sheets);

  std::vector<SheetText> texts_;
  BehaviourRegistry behaviours_;
  std::vector<EntitySheet> sheets_;
};

}  // namespace rtype::game
