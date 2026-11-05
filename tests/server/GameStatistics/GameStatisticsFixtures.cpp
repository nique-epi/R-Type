#include "GameStatisticsFixtures.hpp"
#include <string>
#include <vector>
#include "PluginConstants.hpp"
#include "PluginFiles.hpp"
#include "ServerGameStatisticsModuleTestConstants.hpp"

std::vector<std::string> gameStatisticsPluginLines() {
  return {std::string{rtype::engine::PLUGIN_FOLDER_COMMAND_NAME} + ' ' +
              pluginFolderText(),
          std::string{rtype::engine::PLUGIN_COMMAND_NAME} + ' ' +
              std::string{GAME_STATISTICS_PLUGIN_NAME}};
}
