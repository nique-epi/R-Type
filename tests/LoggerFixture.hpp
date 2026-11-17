#pragma once

#include <gtest/gtest.h>
#include "LogLevel.hpp"

/** @brief Starts each test with the default output of the Logger, and puts the
 * level and the output back afterwards. */
class LoggerFixture : public ::testing::Test {
 protected:
  void SetUp() override;
  void TearDown() override;

 private:
  rtype::logging::LogLevel savedLevel_{rtype::logging::LogLevel::Info};
};
