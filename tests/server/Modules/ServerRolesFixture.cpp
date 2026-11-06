#include "ServerRolesFixture.hpp"
#include <gtest/gtest.h>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <memory>
#include <set>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "Application.hpp"
#include "CommandInterpreter.hpp"
#include "CommandLine.hpp"
#include "CommandResult.hpp"
#include "GameLaunchOptions.hpp"
#include "GameProcessModules.hpp"
#include "GameStatisticsFixtures.hpp"
#include "IModule.hpp"
#include "LaunchArgumentsConstants.hpp"
#include "LaunchOptions.hpp"
#include "LoggingConstants.hpp"
#include "LoopStages.hpp"
#include "ServerConstants.hpp"
#include "ServerModuleTestConstants.hpp"
#include "ServerRolesTestConstants.hpp"
#include "SettingCommandConstants.hpp"
#include "StartRefusedException.hpp"
#include "SupervisorModules.hpp"

ServerRolesFixture::ServerRolesFixture()
    : journalText_(journal().path().string()) {}

std::set<std::string> ServerRolesFixture::supervisorSettings() {
  const std::vector<std::string_view> arguments{
      rtype::logging::FILE_OPTION, journalText_,
      rtype::logging::NO_STDERR_OPTION};
  return declaredSettingsOf(rtype::server::supervisorModules(
                                rtype::server::LaunchOptions::read(arguments),
                                stopRequest_, gameLines_),
                            {});
}

std::set<std::string> ServerRolesFixture::gameSettings() {
  return declaredSettingsOf(gameModules(), gameStatisticsPluginLines());
}

std::vector<std::unique_ptr<rtype::engine::IModule>>
ServerRolesFixture::gameModules() {
  return rtype::server::gameProcessModules(gameOptions(), stopRequest_);
}

std::set<std::string> ServerRolesFixture::exampleSettings() {
  std::ifstream stream{
      std::filesystem::path{RTYPE_SOURCE_SERVER_CONFIGURATION_EXAMPLE}};
  EXPECT_TRUE(stream.is_open()) << RTYPE_SOURCE_SERVER_CONFIGURATION_EXAMPLE;
  std::set<std::string> names;
  std::string line;
  while (std::getline(stream, line)) {
    const std::vector<std::string> words =
        rtype::engine::splitCommandLine(uncommented(line));
    if (words.size() > 1 && words.front() == rtype::engine::SET_COMMAND_NAME) {
      names.insert(words[1]);
    }
  }
  return names;
}

std::set<std::string> ServerRolesFixture::declaredSettingsOf(
    std::vector<std::unique_ptr<rtype::engine::IModule>> modules,
    const std::vector<std::string>& pluginLines) {
  std::set<std::string> names;
  rtype::engine::Application application(
      rtype::engine::LoopStages(rtype::server::SERVER_LOOP_STAGES),
      std::move(modules),
      [&pluginLines, &names](rtype::engine::CommandInterpreter& interpreter) {
        for (const std::string& line : pluginLines) {
          static_cast<void>(interpreter.execute(line));
        }
        names = namesListedIn(interpreter.execute(UNDECLARED_SETTING_LINE));
        if (!names.empty()) {
          throw StartRefusedException();
        }
      });
  EXPECT_THROW(application.start(), StartRefusedException);
  return names;
}

std::set<std::string> ServerRolesFixture::namesListedIn(
    const rtype::engine::CommandResult& refusal) {
  std::set<std::string> names;
  const std::size_t marker = refusal.message.find(KNOWN_SETTINGS_MARKER);
  if (marker == std::string::npos) {
    return names;
  }
  std::string_view rest = std::string_view{refusal.message}.substr(
      marker + KNOWN_SETTINGS_MARKER.size());
  while (!rest.empty()) {
    const std::size_t end = rest.find(KNOWN_SETTINGS_SEPARATOR);
    names.emplace(rest.substr(0, end));
    if (end == std::string_view::npos) {
      break;
    }
    rest.remove_prefix(end + KNOWN_SETTINGS_SEPARATOR.size());
  }
  return names;
}

std::string_view ServerRolesFixture::uncommented(std::string_view line) {
  const std::size_t start = line.find_first_not_of(COMMENT_PREFIX_CHARACTERS);
  return start == std::string_view::npos ? std::string_view{}
                                         : line.substr(start);
}

rtype::server::GameLaunchOptions ServerRolesFixture::gameOptions() const {
  const std::vector<std::string_view> arguments{
      rtype::engine::PORT_OPTION,       ROLE_GAME_PORT,
      rtype::server::SUPERVISOR_OPTION, ROLE_SUPERVISOR_PORT,
      rtype::server::LINK_TOKEN_OPTION, GAME_PROCESS_LINK_TOKEN,
      rtype::logging::FILE_OPTION,      journalText_,
      rtype::logging::NO_STDERR_OPTION};
  return rtype::server::GameLaunchOptions::read(arguments);
}
