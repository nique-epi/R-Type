#pragma once

#include <string_view>
#include "EntityAppearances.hpp"

/** @returns The appearances of one sheet, named READ_TYPE_NAME, holding
 * @p text. */
[[nodiscard]] rtype::client::EntityAppearances appearancesReading(
    std::string_view text);

/** @returns The appearances of the sheets of READ_SHEET_TEXT and
 * OTHER_SHEET_TEXT, named READ_TYPE_NAME and UNREAD_TYPE_NAME, and one more
 * sheet drawing from READ_SHEET_ID. */
[[nodiscard]] rtype::client::EntityAppearances threeAppearancesOnTwoSheets();

/** @returns The appearances of the sheets shipped in assets/. */
[[nodiscard]] rtype::client::EntityAppearances shippedAppearances();
