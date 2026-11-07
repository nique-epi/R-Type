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

  const bool listed = std::ranges::any_of(
      modules, [](const std::unique_ptr<IGameModule>& module) {
        return module->name() == CHAT_MODULE_NAME;
      });

  EXPECT_EQ(listed, CHAT_MODULE_BUILT);
}
