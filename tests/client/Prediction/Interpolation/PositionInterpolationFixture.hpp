#pragma once

#include <gtest/gtest.h>
#include <cstdint>
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"
#include "InterpolationClock.hpp"
#include "Position.hpp"
#include "PositionSample.hpp"

namespace rtype::client::testing {

/**
 * @brief One entity at WHERE_THE_SERVER_PUT_IT with an empty history, and a
 * clock that received no state, to place with placeInterpolatedPositions().
 */
class PositionInterpolationFixture : public ::testing::Test {
 protected:
  PositionInterpolationFixture();

  /** @brief Gives the entity @p sample as its newest recorded position. */
  void record(PositionSample sample);

  /** @brief Tells the clock a state of @p tick arrived. */
  void receiveStateOfTick(std::uint32_t tick);

  /** @brief Places every entity at the tick the clock shows. */
  void place();

  /** @returns Where the entity is now. */
  [[nodiscard]] game::Position position();

 private:
  engine::EntityRegistry entities_;
  engine::ComponentRegistry components_{entities_};
  engine::Entity entity_;
  InterpolationClock clock_;
};

}  // namespace rtype::client::testing
