#include "MasterLaunchOptions.hpp"
#include <array>
#include <functional>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "EndpointText.hpp"
#include "ExecutablePath.hpp"
#include "ForcedSetting.hpp"
#include "LaunchArguments.hpp"
#include "LaunchArgumentsConstants.hpp"
#include "LaunchOption.hpp"
#include "LaunchOptionReader.hpp"
#include "LogLaunchOptions.hpp"
#include "LoggingConstants.hpp"
#include "MasterConstants.hpp"
#include "MasterLaunchValues.hpp"
#include "NetworkConstants.hpp"
#include "SettingLine.hpp"
#include "SettingOptions.hpp"

namespace rtype::master {

namespace {

/**
 * @returns `-p <port>`, which forces the port setting to a checked port.
 */
engine::LaunchOption portOption(MasterLaunchValues& values) {
  return engine::LaunchOption{
      .name = engine::PORT_OPTION,
      .takesValue = true,
      .record = [&values](std::string_view port) {
        values.forcedLines.push_back(engine::settingLine(
            PORT_SETTING_NAME, std::to_string(network::portFromText(port))));
      }};
}

/** @returns The lines of the usage that describe `-p` and `--database`. */
std::string settingOptionEntries() {
  std::string text = engine::usageEntry(
      std::string(engine::PORT_OPTION) + " <port>",
      "TCP port the HTTP API listens on, from " +
          std::to_string(network::LOWEST_PORT) + " to " +
          std::to_string(network::HIGHEST_PORT) +
          ". Default: " + std::to_string(network::DEFAULT_MASTER_PORT) + ".");
  text += engine::usageEntry(
      std::string(engine::DATABASE_OPTION) + " <file>",
      "File the master keeps its data in, on a local disk. Default: " +
          std::string(DATABASE_FILE_NAME) + " next to the executable.");
  return text;
}

/** @returns The lines of the usage that describe `--set` and `--help`. */
std::string generalOptionEntries() {
  std::string text = engine::setOptionEntry(MASTER_CONFIGURATION_FILE_NAME,
                                            MASTER_SETTING_NAMES);
  text += engine::usageEntry(engine::HELP_OPTION,
                             "Print this help and start nothing.");
  return text;
}

}  // namespace

MasterLaunchOptions MasterLaunchOptions::read(
    std::span<const std::string_view> arguments) {
  MasterLaunchOptions options{
      MasterLaunchValues{.forcedLines = {}, .helpRequested = false}};
  const std::array launchOptions{
      portOption(options.values_),
      engine::settingOption(
          engine::ForcedSetting{.option = engine::DATABASE_OPTION,
                                .setting = DATABASE_SETTING_NAME},
          options.values_.forcedLines),
      engine::setOption(options.values_.forcedLines),
      engine::helpOption(options.values_.helpRequested)};
  const std::array<engine::LaunchOptionReader, 1> readers{std::bind_front(
      &logging::LogLaunchOptions::readOption, &options.logOptions_)};
  engine::readLaunchArguments(arguments, launchOptions, readers);
  return options;
}

std::string MasterLaunchOptions::usage() {
  std::string text = engine::usageHeader(MASTER_PROGRAM_NAME);
  text += settingOptionEntries();
  text += generalOptionEntries();
  text += '\n';
  text += logging::LogLaunchOptions::usage(logging::MASTER_LOG_FILE_NAME);
  return text;
}

const std::vector<std::string>& MasterLaunchOptions::forcedLines() const {
  return values_.forcedLines;
}

bool MasterLaunchOptions::helpRequested() const {
  return values_.helpRequested;
}

const logging::LogLaunchOptions& MasterLaunchOptions::logOptions() const {
  return logOptions_;
}

MasterLaunchOptions::MasterLaunchOptions(MasterLaunchValues values)
    : values_(std::move(values)),
      logOptions_(engine::executableFolder() / logging::MASTER_LOG_FILE_NAME) {}

}  // namespace rtype::master
