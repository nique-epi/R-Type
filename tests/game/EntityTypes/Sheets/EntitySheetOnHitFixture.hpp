#pragma once

#include <gtest/gtest.h>
#include <initializer_list>
#include <string>
#include <string_view>
#include "BehaviourRegistry.hpp"
#include "EntitySheet.hpp"
#include "IHitHandler.hpp"

namespace rtype::game::testing {

/**
 * @brief Reads sheets made of pieces of TOML with the built-in behaviours and
 * one hit handler, ON_HIT_HANDLER_NAME, which outlive the sheets read.
 */
class EntitySheetOnHitFixture : public ::testing::Test {
 protected:
  EntitySheetOnHitFixture();

  /** @returns The gameplay of the sheet whose text is @p parts. */
  [[nodiscard]] EntitySheet read(
      std::initializer_list<std::string_view> parts) const;

  /** @returns The message of what reading the sheet of @p parts throws, empty
   * when it throws nothing. */
  [[nodiscard]] std::string refusalOf(
      std::initializer_list<std::string_view> parts) const;

  /** @returns The hit handler of the registry. */
  [[nodiscard]] IHitHandler* handler() const;

 private:
  BehaviourRegistry behaviours_;
  IHitHandler* handler_{nullptr};
};

}  // namespace rtype::game::testing
