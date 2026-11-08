#pragma once

#include <string>
#include <vector>
#include "GameLaunchSettings.hpp"

namespace rtype::server {

/**
 * @returns The names of the difficulties of the waves file of @p settings, in
 * alphabetical order, read with the entity sheets of its content folders,
 * which it names its enemies among. The server scripts of those folders run
 * first, so a sheet may name a behaviour a script registers; nothing steers
 * here, so their failures are not reported.
 * @throws What BehaviourScripts::run(), sheetTextsInFolders(),
 * EntitySheets::fromTexts() and WaveCatalog::fromFile() throw.
 */
[[nodiscard]] std::vector<std::string> difficultyNamesOf(
    const GameLaunchSettings& settings);

}  // namespace rtype::server
