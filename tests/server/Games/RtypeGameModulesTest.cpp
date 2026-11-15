#include <gtest/gtest.h>
#include <algorithm>
#include <memory>
#include <vector>
#include "BuiltModuleConstants.hpp"
#include "GameInstanceConstants.hpp"
#include "IGameModule.hpp"
#include "RecordingResultsArchive.hpp"
#include "RtypeGameModules.hpp"

using rtype::engine::CASINO_MODULE_BUILT;
using rtype::engine::CHAT_MODULE_BUILT;
using rtype::server::CASINO_MODULE_NAME;
using rtype::server::CHAT_MODULE_NAME;
using rtype::server::IGameModule;
using rtype::server::rtypeGameModules;

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
 * Given the modules of an R-Type game, in a build with or without the casino
 * When they are listed
 * Then the casino is among them exactly when the build compiled it
 */
TEST(RtypeGameModules, ListsTheCasinoOnlyWhenItIsBuilt) {
  RecordingResultsArchive results;
  const std::vector<std::unique_ptr<IGameModule>> modules =
      rtypeGameModules(results);

  const bool listed = std::ranges::find(modules, CASINO_MODULE_NAME,
                                        &IGameModule::name) != modules.end();

  EXPECT_EQ(listed, CASINO_MODULE_BUILT);
}
