#pragma once

#include <string_view>
#include "SheetText.hpp"

namespace rtype::game::testing {

/** @returns The sheet @p name whose file holds @p text, the errors naming it
 * `<name>.toml`: what sheetTextsIn() would read from such a file. */
[[nodiscard]] SheetText sheetText(std::string_view name, std::string_view text);

}  // namespace rtype::game::testing
