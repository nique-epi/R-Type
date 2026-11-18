#pragma once

#include <filesystem>
#include <string_view>

/** @returns The file @p assetId names in the assets folder of the
 * repository. */
[[nodiscard]] std::filesystem::path shippedFile(std::string_view assetId);

/** @returns The LDtk project of the tests, which holds a prop. */
[[nodiscard]] std::filesystem::path propProject();
