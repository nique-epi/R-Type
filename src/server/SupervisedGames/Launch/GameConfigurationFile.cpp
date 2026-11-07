#include "GameConfigurationFile.hpp"
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <ios>
#include <span>
#include <string>
#include "PathText.hpp"
#include "ServerException.hpp"
#include "SupervisedGamesConstants.hpp"

namespace rtype::server {

std::filesystem::path gameConfigurationFileOf(
    const std::filesystem::path& folder, std::uint16_t supervisorPort) {
  return folder / (std::string{GAME_CONFIGURATION_FILE_PREFIX} +
                   std::to_string(supervisorPort) +
                   std::string{GAME_CONFIGURATION_FILE_SUFFIX});
}

void writeGameConfiguration(const std::filesystem::path& file,
                            std::span<const std::string> lines) {
  std::ofstream stream{file, std::ios::trunc};
  for (const std::string& line : lines) {
    stream << line << '\n';
  }
  stream.flush();
  if (!stream) {
    throw GameConfigurationWriteException(logging::genericText(file));
  }
}

}  // namespace rtype::server
