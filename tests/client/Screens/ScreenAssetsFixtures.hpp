#pragma once

#include <filesystem>
#include <string_view>
#include "EntityAppearances.hpp"

/** @returns The assets folder of the repository, RTYPE_ASSETS_FOLDER, which
 * client_screen_tests defines. */
[[nodiscard]] std::filesystem::path repositoryAssets();

/** @returns The appearance of one sheet, drawn from the sprite sheet
 * @p sheetId, as EntityAppearances reads it. */
[[nodiscard]] rtype::client::EntityAppearances appearanceDrawnFrom(
    std::string_view sheetId);
