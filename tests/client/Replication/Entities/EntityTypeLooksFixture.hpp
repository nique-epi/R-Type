#pragma once

#include <gtest/gtest.h>
#include "EntityAppearances.hpp"
#include "EntityTypeLooks.hpp"
#include "SessionEntityTypes.hpp"

namespace rtype::client::testing {

/** @brief The looks of a table of two types, LOOKED_UP_TYPE_NAME, an enemy,
 * then TYPE_WITHOUT_APPEARANCE_NAME, an enemy too, joined with the appearance
 * of the first alone, the table judged by a client that draws every type. */
class EntityTypeLooksFixture : public ::testing::Test {
 protected:
  EntityTypeLooksFixture();

  [[nodiscard]] const EntityTypeLooks& looks() const;

  [[nodiscard]] const EntityAppearances& appearances() const;

 private:
  SessionEntityTypes table_;
  EntityAppearances appearances_;
  EntityTypeLooks looks_{table_, appearances_};
};

}  // namespace rtype::client::testing
