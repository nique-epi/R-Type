#include <gtest/gtest.h>
#include <string>
#include <string_view>
#include <vector>
#include "LaunchOptions.hpp"
#include "ModuleConstants.hpp"
#include "StopRequest.hpp"
#include "SupervisorModules.hpp"

using rtype::server::LaunchOptions;
using rtype::server::StopRequest;
using rtype::server::supervisorModules;

/**
 * Given the launch options of a supervisor with no argument
 * When its modules are listed
 * Then they are the journal, the network, the supervised games and the lobby
 * that sends the players to them, in the order they start
 */
TEST(SupervisorModules, ListsTheModulesOfTheSupervisor) {
  StopRequest stopRequest;
  const LaunchOptions options =
      LaunchOptions::read(std::vector<std::string_view>{});

  const auto modules = supervisorModules(options, stopRequest);

  std::vector<std::string> names;
  names.reserve(modules.size());
  for (const auto& listed : modules) {
    names.push_back(listed->name());
  }
  EXPECT_EQ(names,
            (std::vector<std::string>{
                std::string{rtype::server::LOGGER_MODULE_NAME},
                std::string{rtype::server::NETWORK_MODULE_NAME},
                std::string{rtype::server::SUPERVISED_GAMES_MODULE_NAME},
                std::string{rtype::server::SUPERVISOR_LOBBY_MODULE_NAME}}));
}
