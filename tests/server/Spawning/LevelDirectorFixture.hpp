#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string_view>
#include <vector>
#include "BehaviourScripts.hpp"
#include "EventBus.hpp"
#include "LevelChange.hpp"
#include "LevelDirector.hpp"
#include "Position.hpp"
#include "World.hpp"

namespace rtype::server::testing {

/**
 * @brief A world of the shipped sheets and the LevelDirector of the level a
 * test script registers, ticked as a game ticks it; the changes of the level
 * and the removals it publishes are recorded once the bus delivers them.
 */
class LevelDirectorFixture : public ::testing::Test {
 protected:
  LevelDirectorFixture();

  /** @brief Runs @p script and directs the level DIRECTED_LEVEL it
   * registers. */
  void direct(std::string_view script);
  /** @brief Lets @p count ticks pass, the bus delivering after each. */
  void tick(std::size_t count);
  /** @returns The network id of a body of @p sheet put at @p position. */
  std::uint32_t place(std::string_view sheet, game::Position position);
  /** @returns The network id of a body of @p sheet put at @p position, made to
   * follow the scroll as a sheet whose motion is scroll makes it. */
  std::uint32_t placeFollowingTheScroll(std::string_view sheet,
                                        game::Position position);
  /** @returns Where the body of @p networkId is; the origin when it is gone. */
  [[nodiscard]] game::Position positionOf(std::uint32_t networkId);
  /** @returns The director of the level. */
  [[nodiscard]] LevelDirector& director();
  /** @returns The scripts of the test. */
  [[nodiscard]] game::BehaviourScripts& scripts();
  /** @returns The changes of the level published so far. */
  [[nodiscard]] const std::vector<game::LevelChange>& changes() const;
  /** @returns The network ids of the bodies removed so far. */
  [[nodiscard]] const std::vector<std::uint32_t>& removed() const;

 private:
  game::World world_;
  engine::EventBus events_;
  game::BehaviourScripts scripts_;
  std::unique_ptr<LevelDirector> director_;
  std::vector<game::LevelChange> changes_;
  std::vector<std::uint32_t> removed_;
};

}  // namespace rtype::server::testing
