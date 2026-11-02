#include "ClientConfiguration.hpp"
#include <filesystem>
#include <string>
#include <vector>
#include "ClientConstants.hpp"
#include "CommandInterpreter.hpp"
#include "ConfigurationReader.hpp"
#include "ConfigurationReport.hpp"
#include "ConfigurationStep.hpp"
#include "LaunchOptions.hpp"

namespace rtype::client {

engine::ConfigurationStep clientConfiguration(
    const LaunchOptions& launchOptions, engine::ConfigurationReport& report) {
  return [forcedLines = launchOptions.forcedLines(),
          &report](engine::CommandInterpreter& interpreter) {
    report = engine::readConfiguration(
        std::filesystem::path{CLIENT_CONFIGURATION_FILE_NAME}, forcedLines,
        interpreter);
  };
}

}  // namespace rtype::client
