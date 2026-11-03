#pragma once

#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "DebugJournal.hpp"

namespace rtype::network::testing {

/** @brief Dispatcher tests that read the log the dispatcher writes. */
class MessageDispatcherLogFixture : public ::testing::Test {
 protected:
  /** @returns The lines of the journal about a rejected message. */
  [[nodiscard]] std::vector<std::string> rejectionLines() const;

  [[nodiscard]] const DebugJournal& journal() const;

 private:
  DebugJournal journal_;
};

}  // namespace rtype::network::testing
