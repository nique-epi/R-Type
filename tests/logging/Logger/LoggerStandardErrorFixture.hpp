#pragma once

#include <gtest/gtest.h>
#include <sstream>
#include <streambuf>
#include <string>
#include "LogLevel.hpp"

/** @brief Starts each test at the info level, writing to standard error alone,
 * keeps what reaches std::cerr, and puts the level, the output of the Logger
 * and std::cerr back afterwards. */
class LoggerStandardErrorFixture : public ::testing::Test {
 protected:
  void SetUp() override;
  void TearDown() override;

  /** @returns What reached std::cerr since the test started. */
  [[nodiscard]] std::string standardErrorStream() const;

 private:
  rtype::logging::LogLevel savedLevel_{rtype::logging::LogLevel::Info};
  std::ostringstream standardError_;
  std::streambuf* originalBuffer_{nullptr};
};
