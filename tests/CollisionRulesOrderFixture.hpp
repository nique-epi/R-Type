#pragma once

#include <gtest/gtest.h>
#include "Collision.hpp"
#include "Entity.hpp"

/**
 * @brief Runs a test twice, the two entities of its Collision named in one
 * order, then in the other: the parameter is true for the reverse order.
 */
class CollisionRulesOrderFixture : public ::testing::TestWithParam<bool> {
 protected:
  /** @returns A Collision of @p entity and @p other, in the order of the
   * parameter. */
  [[nodiscard]] static rtype::engine::Collision collisionOf(
      rtype::engine::Entity entity, rtype::engine::Entity other);
};
