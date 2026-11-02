#pragma once

#include <gtest/gtest.h>
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

 private:
  rtype::logging::LogLevel savedLevel_{rtype::logging::LogLevel::Info};
  std::ostringstream standardError_;
  std::streambuf* originalBuffer_{nullptr};
};
