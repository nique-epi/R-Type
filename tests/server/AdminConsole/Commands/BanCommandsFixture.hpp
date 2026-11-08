#pragma once

#include <gtest/gtest.h>
#include <chrono>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>
#include "BanBook.hpp"
#include "BanCommands.hpp"
#include "CommandInterpreter.hpp"
#include "CommandResult.hpp"
#include "RecordingConsoleOutput.hpp"
#include "TemporaryFolder.hpp"

/** @brief The ban commands registered in an interpreter of their own, over a
 * book open on a file of the test, at a moment the tests move: BAN_MOMENT
 * first. */
class BanCommandsFixture : public ::testing::Test {
 protected:
  BanCommandsFixture();

  /** @returns What the interpreter did with @p line. */
  [[nodiscard]] rtype::engine::CommandResult run(std::string_view line);

  /** @brief Moves the time the commands read by @p elapsed. */
  void wait(std::chrono::seconds elapsed);

  [[nodiscard]] const rtype::server::BanBook& book() const;
  [[nodiscard]] std::filesystem::path file() const;
  [[nodiscard]] std::vector<std::string> lines() const;

 private:
  TemporaryFolder folder_;
  rtype::server::BanBook book_;
  rtype::engine::CommandInterpreter interpreter_;
  RecordingConsoleOutput output_;
  std::chrono::sys_seconds now_;
  rtype::server::BanCommands commands_;
};
