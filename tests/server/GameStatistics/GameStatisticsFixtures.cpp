#include "GameStatisticsFixtures.hpp"
#include <filesystem>
#include <string>
#include <vector>
#include "PathText.hpp"
#include "PluginConstants.hpp"
#include "ServerGameStatisticsModuleTestConstants.hpp"

std::vector<std::string> gameStatisticsPluginLines() {
  return {std::string{rtype::engine::PLUGIN_FOLDER_COMMAND_NAME} + ' ' +
              rtype::logging::genericText(
                  std::filesystem::path{RTYPE_GAME_STATISTICS_PLUGIN_FOLDER}),
          std::string{rtype::engine::PLUGIN_COMMAND_NAME} + ' ' +
              std::string{GAME_STATISTICS_PLUGIN_NAME}};
}
