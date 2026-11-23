#include <gtest/gtest.h>
#include <algorithm>
#include <memory>
#include <vector>
#include "BuiltModuleConstants.hpp"
#include "GameInstanceConstants.hpp"
#include "IGameModule.hpp"
#include "RecordingResultsArchive.hpp"
#include "RtypeGameModules.hpp"

using rtype::engine::CHAT_MODULE_BUILT;
using rtype::engine::THREAT_MODULE_BUILT;
using rtype::server::CHAT_MODULE_NAME;
using rtype::server::IGameModule;
using rtype::server::RTYPE_RULES_MODULE_NAME;
using rtype::server::rtypeGameModules;
using rtype::server::THREAT_MODULE_NAME;

/**
 * Given the modules of an R-Type game, in a build with or without the chat
 * When they are listed
 * Then the chat is among them exactly when the build compiled it
 */
TEST(RtypeGameModules, ListsTheChatOnlyWhenItIsBuilt) {
  RecordingResultsArchive results;
  const std::vector<std::unique_ptr<IGameModule>> modules =
      rtypeGameModules(results);

  const bool listed = std::ranges::find(modules, CHAT_MODULE_NAME,
                                        &IGameModule::name) != modules.end();

  EXPECT_EQ(listed, CHAT_MODULE_BUILT);
}

/**
 * Given the modules of an R-Type game, in a build with or without the threat
 * When they are listed
 * Then the threat is among them exactly when the build compiled it
 */
TEST(RtypeGameModules, ListsTheThreatOnlyWhenItIsBuilt) {
  RecordingResultsArchive results;
  const std::vector<std::unique_ptr<IGameModule>> modules =
      rtypeGameModules(results);

  const bool listed = std::ranges::find(modules, THREAT_MODULE_NAME,
                                        &IGameModule::name) != modules.end();

  EXPECT_EQ(listed, THREAT_MODULE_BUILT);
}

/**
 * Given the modules of an R-Type game, in a build with the threat
 * When they are listed
 * Then the threat comes before the rules, which find its service
 */
TEST(RtypeGameModules, ListsTheThreatBeforeTheRules) {
  if (!THREAT_MODULE_BUILT) {
    GTEST_SKIP();
  }
  RecordingResultsArchive results;
  const std::vector<std::unique_ptr<IGameModule>> modules =
      rtypeGameModules(results);

  const auto threat =
      std::ranges::find(modules, THREAT_MODULE_NAME, &IGameModule::name);
  const auto rules =
      std::ranges::find(modules, RTYPE_RULES_MODULE_NAME, &IGameModule::name);

  EXPECT_LT(threat, rules);
}
