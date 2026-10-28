#pragma once

#include <gtest/gtest.h>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
#include "JournalFile.hpp"
#include "LogLaunchOptions.hpp"

/** @brief The base of the tests of the modules of the server: a journal file
 * the logging options point to, and a free UDP port. */
class ServerModuleFixture : public ::testing::Test {
 protected:
  ServerModuleFixture();

  /** @returns Logging options that write to the journal of the test only. */
  [[nodiscard]] rtype::logging::LogLaunchOptions journalOptions() const;

  /** @returns A UDP port nothing was bound to a moment ago. */
  [[nodiscard]] static std::uint16_t freeUdpPort();

  JournalFile journal_;

 private:
  std::string journalPath_;
  std::vector<std::string_view> arguments_;
};
