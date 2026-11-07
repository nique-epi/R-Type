#pragma once

#include <filesystem>
#include "EntitySheets.hpp"

namespace rtype::game::testing {

/** @returns The assets folder of the sources: assets/ of the repository. */
[[nodiscard]] std::filesystem::path shippedAssetFolder();

/** @returns The folder of the sheets the game ships with, in the sources:
 * assets/entities/ of the repository. */
[[nodiscard]] std::filesystem::path shippedSheetFolder();

/**
 * @returns The sheets the game ships with, read once from
 * shippedSheetFolder() the first time it is called, then kept for the rest of
 * the test program: tests that build entities from the real types share them,
 * as a server shares its catalog between its games.
 * @throws What EntitySheets::fromFolder() throws, when a shipped sheet is
 * wrong.
 */
[[nodiscard]] const EntitySheets& shippedEntitySheets();

}  // namespace rtype::game::testing
