#pragma once

#include <gtest/gtest.h>
#include <string>
#include "CerrCapture.hpp"
#include "LogLevel.hpp"

/** @brief Starts each test at the warn level with no output, keeps what reaches
 * standard error, and puts the Logger back afterwards. */
class LogLaunchOptionsFixture : public ::testing::Test {
 protected:
  void SetUp() override;
  void TearDown() override;

  [[nodiscard]] std::string standardError() const;

 private:
  rtype::logging::LogLevel savedLevel_{rtype::logging::LogLevel::Info};
  CerrCapture standardError_;
};
