#include "GameStatisticsFixtures.hpp"
#include <cstdint>
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

std::string statisticsFileOfPort(std::uint16_t port) {
  return std::string{STATISTICS_FILE_STEM} +
         std::string{STATISTICS_PORT_SEPARATOR} + std::to_string(port) +
         std::string{STATISTICS_FILE_EXTENSION};
}
