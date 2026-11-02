#pragma once

#include <gtest/gtest.h>
#include <vector>
#include "BasicScreenRegistry.hpp"
#include "RecordingScreen.hpp"
#include "ScreenId.hpp"
#include "ScreenStack.hpp"

/** @brief A registry whose context is an int, and a stack that asks it for
 * screens, both registering recording screens in one journal. */
class ScreenRegistryFixture : public ::testing::Test {
 protected:
  /** @brief Registers a recording screen under @p screen, as a module does in
   * its install(). */
  void registerRecordingScreen(rtype::client::ScreenId screen);

  rtype::client::BasicScreenRegistry<int>& registry();
  rtype::client::ScreenStack& stack();
  std::vector<JournalEntry>& journal();

 private:
  std::vector<JournalEntry> journal_;
  rtype::client::BasicScreenRegistry<int> registry_;
  rtype::client::ScreenStack stack_{[this](rtype::client::ScreenId screen) {
    return registry_.create(screen, 0);
  }};
};
