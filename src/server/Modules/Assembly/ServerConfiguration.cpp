#include "ServerConfiguration.hpp"
#include <filesystem>
#include <string>
#include <vector>
#include "CommandInterpreter.hpp"
#include "ConfigurationReader.hpp"
#include "ConfigurationReport.hpp"
#include "ConfigurationStep.hpp"
#include "LaunchOptions.hpp"
#include "ServerConstants.hpp"

namespace rtype::server {

engine::ConfigurationStep serverConfiguration(
    const LaunchOptions& launchOptions, engine::ConfigurationReport& report) {
  return [forcedLines = launchOptions.forcedLines(),
          &report](engine::CommandInterpreter& interpreter) {
    report = engine::readConfiguration(
        std::filesystem::path{SERVER_CONFIGURATION_FILE_NAME}, forcedLines,
        interpreter);
  };
}

}  // namespace rtype::server
