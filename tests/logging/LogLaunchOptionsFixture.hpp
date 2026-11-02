#pragma once

#include <gtest/gtest.h>
#include <filesystem>
#include <sstream>
#include <streambuf>
#include <string>
#include "Logger.hpp"

/** @brief Starts each test at the warn level with no output, keeps what reaches
 * standard error, and puts the Logger back afterwards. */
class LogLaunchOptionsFixture : public ::testing::Test {
 protected:
  void SetUp() override;
  void TearDown() override;

  [[nodiscard]] std::string standardError() const;

  /** @returns A journal that cannot be opened, its folder being missing. */
  [[nodiscard]] static std::filesystem::path missingJournalPath();

 private:
  rtype::logging::LogLevel savedLevel_{rtype::logging::LogLevel::Info};
  std::ostringstream standardError_;
  std::streambuf* originalBuffer_{nullptr};
};
