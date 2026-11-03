#include "ScreenRegistryFixture.hpp"
#include <memory>
#include <vector>
#include "BasicScreenRegistry.hpp"
#include "RecordingScreen.hpp"
#include "ScreenId.hpp"
#include "ScreenStack.hpp"

using rtype::client::BasicScreenRegistry;
using rtype::client::ScreenId;
using rtype::client::ScreenStack;

void ScreenRegistryFixture::registerRecordingScreen(ScreenId screen) {
  registry_.add(screen, [this, screen](const int&) {
    return std::make_unique<RecordingScreen>(screen, journal_);
  });
}

BasicScreenRegistry<int>& ScreenRegistryFixture::registry() {
  return registry_;
}

ScreenStack& ScreenRegistryFixture::stack() { return stack_; }

std::vector<JournalEntry>& ScreenRegistryFixture::journal() { return journal_; }
