#include <gtest/gtest.h>
#include <cstdint>
#include "Entity.hpp"
#include "NetworkEntityTable.hpp"
#include "NetworkIdConstants.hpp"

using rtype::engine::Entity;
using rtype::engine::NetworkEntityTable;

namespace {

constexpr std::uint32_t BOUND_ID = 7;
constexpr std::uint32_t UNKNOWN_ID = 8;
constexpr Entity FIRST_ENTITY{.index = 3, .generation = 1};
constexpr Entity SECOND_ENTITY{.index = 4, .generation = 1};

}  // namespace

/**
 * Given a table where an identifier is bound to an entity
 * When the identifier is looked up
 * Then the entity is found
 */
TEST(NetworkEntityTable, BoundIdentifierFindsItsEntity) {
  NetworkEntityTable table;
  table.bind(BOUND_ID, FIRST_ENTITY);

  EXPECT_EQ(table.find(BOUND_ID), FIRST_ENTITY);
}

/**
 * Given a table with one binding
 * When an unknown identifier and the reserved identifier are looked up
 * Then both give an empty result, without throwing
 */
TEST(NetworkEntityTable, UnknownIdentifierGivesNothing) {
  NetworkEntityTable table;
  table.bind(BOUND_ID, FIRST_ENTITY);

  EXPECT_FALSE(table.find(UNKNOWN_ID).has_value());
  EXPECT_FALSE(table.find(rtype::engine::NO_NETWORK_ID).has_value());
}

/**
 * Given an identifier already bound to an entity
 * When it is bound again to another entity
 * Then bind reports false and the first entity is kept
 */
TEST(NetworkEntityTable, BindingAKnownIdentifierChangesNothing) {
  NetworkEntityTable table;
  table.bind(BOUND_ID, FIRST_ENTITY);

  const bool bound = table.bind(BOUND_ID, SECOND_ENTITY);

  EXPECT_FALSE(bound);
  EXPECT_EQ(table.find(BOUND_ID), FIRST_ENTITY);
  EXPECT_EQ(table.size(), 1U);
}

/**
 * Given a table with one binding
 * When the identifier is unbound
 * Then it is no longer found and a second unbind reports false
 */
TEST(NetworkEntityTable, UnboundIdentifierIsForgotten) {
  NetworkEntityTable table;
  table.bind(BOUND_ID, FIRST_ENTITY);

  EXPECT_TRUE(table.unbind(BOUND_ID));
  EXPECT_FALSE(table.find(BOUND_ID).has_value());
  EXPECT_FALSE(table.unbind(BOUND_ID));
}

/**
 * Given an empty table
 * When the reserved identifier is bound
 * Then bind reports false, nothing is stored and nothing is thrown
 */
TEST(NetworkEntityTable, ReservedIdentifierIsNotBound) {
  NetworkEntityTable table;

  EXPECT_FALSE(table.bind(rtype::engine::NO_NETWORK_ID, FIRST_ENTITY));
  EXPECT_EQ(table.size(), 0U);
  EXPECT_FALSE(table.find(rtype::engine::NO_NETWORK_ID).has_value());
}
