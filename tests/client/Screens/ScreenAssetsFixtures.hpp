#pragma once

#include <filesystem>

/** @returns The assets folder of the repository, RTYPE_ASSETS_FOLDER, which
 * client_screen_tests defines. */
[[nodiscard]] std::filesystem::path repositoryAssets();
