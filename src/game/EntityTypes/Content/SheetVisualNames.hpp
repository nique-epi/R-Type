#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include "DataFileTable.hpp"

namespace rtype::game {

/**
 * @brief The names of what a server may ask the client to draw for the
 * bodies of one sheet: its effects, played once, and its looks, idle loops
 * taken instead of its own. Both lists are in the order of the names.
 *
 * The server checks the names a script asks for against it, and numbers a
 * look by lookNumberOf(); the client reads the same sheet, so the same name
 * has the same number on both sides.
 */
struct SheetVisualNames {
  std::vector<std::string> effects;
  std::vector<std::string> looks;
};

/**
 * @returns The names of the tables of `effects` and `looks` of @p appearance,
 * the `[appearance]` table of a sheet, each table left out naming none.
 * Nothing else of the table is read.
 * @throws engine::DataFileTableExpectedException when `effects` or `looks`
 * holds something else than a table of tables, and
 * InconsistentEntitySheetException for a name that is not a name of the
 * content (VISUAL_NAME_RULE), or past MOST_EFFECTS effects or MOST_LOOKS
 * looks.
 */
[[nodiscard]] SheetVisualNames visualNamesOf(
    const engine::DataFileTable& appearance);

/**
 * @returns Whether @p names holds the effect @p effect.
 */
[[nodiscard]] bool namesEffect(const SheetVisualNames& names,
                               std::string_view effect);

/**
 * @returns The number of the look @p look of @p names: its place in the order
 * of the names, from 1, or 0 for the empty name, the idle loop of the sheet
 * itself; nothing when @p names has no such look.
 */
[[nodiscard]] std::optional<std::uint8_t> lookNumberOf(
    const SheetVisualNames& names, std::string_view look);

}  // namespace rtype::game
