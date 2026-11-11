#pragma once

#include <string_view>
#include "SheetVisualNames.hpp"

namespace rtype::game::testing {

/** @returns The visual names of the `[appearance]` table of the sheet @p text
 * holds, read as visualNamesOf() reads them, the errors naming the file
 * `drifter.toml`; throws what parsing the sheet or visualNamesOf() throws. */
[[nodiscard]] SheetVisualNames visualNamesIn(std::string_view text);

}  // namespace rtype::game::testing
