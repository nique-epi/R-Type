#pragma once

#include <gtest/gtest.h>
#include <initializer_list>
#include <string>
#include <string_view>
#include "BehaviourRegistry.hpp"
#include "EntitySheet.hpp"

namespace rtype::game::testing {

/**
 * @brief Reads sheets made of pieces of TOML with the built-in behaviours,
 * which outlive the sheets read, so the pilot of a sheet stays valid in a
 * test.
 */
class EntitySheetReaderFixture : public ::testing::Test {
 protected:
  EntitySheetReaderFixture();

  /** @returns The gameplay of the sheet READ_SHEET_NAME whose text is
   * @p parts, one after the other. */
  [[nodiscard]] EntitySheet read(
      std::initializer_list<std::string_view> parts) const;

  /** @returns The message of what reading the sheet of @p parts throws, empty
   * when it throws nothing. */
  [[nodiscard]] std::string refusalOf(
      std::initializer_list<std::string_view> parts) const;

 private:
  BehaviourRegistry behaviours_;
};

}  // namespace rtype::game::testing
