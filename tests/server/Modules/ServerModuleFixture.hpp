#pragma once

#include <gtest/gtest.h>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
#include "JournalFile.hpp"
#include "LogLaunchOptions.hpp"

/** @brief The base of the tests of the modules of the server: a journal file
 * the logging options point to, and the setting line that has the system
 * choose the port of the server. */
class ServerModuleFixture : public ::testing::Test {
 protected:
  ServerModuleFixture();

  /** @returns Logging options that write to the journal of the test only. */
  [[nodiscard]] rtype::logging::LogLaunchOptions journalOptions() const;

  /** @returns @p lines followed by the line that sets the `port` of the
   * server to ANY_PORT, so the system chooses a free one when it binds. */
  [[nodiscard]] static std::vector<std::string> onAnyPort(
      std::vector<std::string> lines);

  /** @returns Whether a UDP socket can be bound on @p port now. */
  [[nodiscard]] static bool portIsFree(std::uint16_t port);

  /** @brief Makes the logging options write the journal as JSON Lines, for a
   * test that reads its events; called before journalOptions(). */
  void writeTheJournalAsJson();

  /** @returns The journal file the logging options point to. */
  [[nodiscard]] const JournalFile& journal() const;

 private:
  JournalFile journal_;
  std::string journalPath_;
  std::vector<std::string_view> arguments_;
};
