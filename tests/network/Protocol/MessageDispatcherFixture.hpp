#pragma once

#include <gtest/gtest.h>
#include <string>
#include <string_view>
#include <vector>
#include "DebugJournal.hpp"

namespace rtype::network::testing {

/** @brief Dispatcher tests that read the log the dispatcher writes. */
class MessageDispatcherFixture : public ::testing::Test {
 protected:
  /** @returns The lines of the journal about a rejected message. */
  [[nodiscard]] std::vector<std::string> rejectionLines() const;

  /** @returns The lines of the journal that hold @p text, oldest first. */
  [[nodiscard]] std::vector<std::string> linesHolding(
      std::string_view text) const;

 private:
  DebugJournal journal_;
};

}  // namespace rtype::network::testing
