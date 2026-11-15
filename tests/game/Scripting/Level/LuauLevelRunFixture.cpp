#include "LuauLevelRunFixture.hpp"
#include <gtest/gtest.h>
#include <cstddef>
#include <memory>
#include <string_view>
#include <utility>
#include <vector>
#include "BehaviourScriptsFixtures.hpp"
#include "EntityType.hpp"
#include "LdtkPlacement.hpp"
#include "LevelProgress.hpp"
#include "LuauLevelEntry.hpp"
#include "LuauLevelRun.hpp"
#include "LuauLevelRunParts.hpp"
#include "LuauLevelRunTestConstants.hpp"
#include "TimeConstants.hpp"

LuauLevelRunFixture::LuauLevelRunFixture()
    : scripts_(reportInto(log_)), components_(entities_) {}

void LuauLevelRunFixture::placeEnemies(
    std::vector<rtype::game::LdtkPlacement> placements) {
  placements_ = std::move(placements);
}

void LuauLevelRunFixture::play(std::string_view script) {
  scripts_.run(scriptsOf(script));
  rtype::game::LuauLevelEntry* const entry =
      scripts_.levels().entryOf(TEST_LEVEL);
  ASSERT_NE(entry, nullptr);
  entry->placements = std::move(placements_);
  run_ = std::make_unique<rtype::game::LuauLevelRun>(
      rtype::game::LuauLevelRunParts{.machine = scripts_.machine(),
                                     .levels = scripts_.levels(),
                                     .entry = *entry,
                                     .progress = progress_,
                                     .components = components_});
}

void LuauLevelRunFixture::tick(std::size_t count) {
  for (std::size_t passed = 0; passed < count; ++passed) {
    scripts_.machine().beginTick();
    run().step();
    progress_.scroll.advance(rtype::engine::SIMULATION_TICK_DURATION);
  }
}

void LuauLevelRunFixture::addEnemy() {
  components_.add(entities_.create(), rtype::game::EntityType::Enemy);
}

rtype::game::LuauLevelRun& LuauLevelRunFixture::run() { return *run_; }

rtype::game::LevelProgress& LuauLevelRunFixture::progress() {
  return progress_;
}
