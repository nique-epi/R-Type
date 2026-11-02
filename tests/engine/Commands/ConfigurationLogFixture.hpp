#pragma once

#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "JournalFile.hpp"
#include "Logger.hpp"

/**
 * @brief A journal that takes the info lines of a logger, and the default
 * logger output and level back when the test ends.
 */
class ConfigurationLogFixture : public ::testing::Test {
 protected:
  ConfigurationLogFixture();
  ~ConfigurationLogFixture() override;

  ConfigurationLogFixture(const ConfigurationLogFixture&) = delete;
  ConfigurationLogFixture& operator=(const ConfigurationLogFixture&) = delete;
  ConfigurationLogFixture(ConfigurationLogFixture&&) = delete;
  ConfigurationLogFixture& operator=(ConfigurationLogFixture&&) = delete;

  /** @returns The lines the journal holds. */
  [[nodiscard]] std::vector<std::string> lines() const;

  rtype::logging::Logger logger_;

 private:
  JournalFile journal_;
  rtype::logging::LogLevel previousLevel_;
};
