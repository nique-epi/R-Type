#include "DifficultyNames.hpp"
#include <string>
#include <vector>
#include "BehaviourScriptFiles.hpp"
#include "BehaviourScripts.hpp"
#include "EntitySheets.hpp"
#include "GameLaunchSettings.hpp"
#include "SheetTextReading.hpp"
#include "WaveCatalog.hpp"

namespace rtype::server {

std::vector<std::string> difficultyNamesOf(const GameLaunchSettings& settings) {
  game::BehaviourScripts scripts{[](const std::string&) {}};
  scripts.run(game::scriptSourcesIn(settings.contentFolders));
  const game::EntitySheets types = game::EntitySheets::fromTexts(
      game::sheetTextsInFolders(settings.contentFolders), scripts.behaviours());
  return WaveCatalog::fromFile(settings.wavesFile, types).difficulties();
}

}  // namespace rtype::server
