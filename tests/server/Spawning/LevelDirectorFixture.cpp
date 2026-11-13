#include "LevelDirectorFixture.hpp"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include "BehaviourScripts.hpp"
#include "EntityDestruction.hpp"
#include "LevelChange.hpp"
#include "LevelDirector.hpp"
#include "LevelDirectorParts.hpp"
#include "LevelDirectorTestConstants.hpp"
#include "Position.hpp"
#include "ScriptSource.hpp"
#include "ShippedEntitySheets.hpp"

namespace rtype::server::testing {

LevelDirectorFixture::LevelDirectorFixture()
    : scripts_([](const std::string&) {}) {
  static_cast<void>(events_.subscribe<game::LevelChange>(
      [this](const game::LevelChange& change) { changes_.push_back(change); }));
  static_cast<void>(events_.subscribe<game::EntityDestruction>(
      [this](const game::EntityDestruction& removal) {
        removed_.push_back(removal.networkId);
      }));
}

void LevelDirectorFixture::direct(std::string_view script) {
  const std::vector<engine::ScriptSource> sources{engine::ScriptSource{
      .name = std::string{DIRECTED_LEVEL}, .text = std::string{script}}};
  scripts_.run(sources);
  director_ = std::make_unique<LevelDirector>(
      LevelDirectorParts{.world = world_,
                         .events = events_,
                         .scripts = scripts_,
                         .level = std::string{DIRECTED_LEVEL}});
}

void LevelDirectorFixture::tick(std::size_t count) {
  for (std::size_t passed = 0; passed < count; ++passed) {
    scripts_.machine().beginTick();
    director().onTick();
    events_.dispatch();
  }
}

std::uint32_t LevelDirectorFixture::place(std::string_view sheet,
                                          game::Position position) {
  return world_.spawn(game::testing::shippedEntitySheets().named(sheet),
                      position);
}

LevelDirector& LevelDirectorFixture::director() { return *director_; }

game::BehaviourScripts& LevelDirectorFixture::scripts() { return scripts_; }

const std::vector<game::LevelChange>& LevelDirectorFixture::changes() const {
  return changes_;
}

const std::vector<std::uint32_t>& LevelDirectorFixture::removed() const {
  return removed_;
}

}  // namespace rtype::server::testing
