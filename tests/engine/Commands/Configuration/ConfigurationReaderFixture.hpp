#pragma once

#include <gtest/gtest.h>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>
#include "CommandInterpreter.hpp"
#include "ConfigurationRegistry.hpp"
#include "ConfigurationReport.hpp"
#include "ConfigurationSource.hpp"
#include "SettingValue.hpp"
#include "TemporaryFolder.hpp"

/**
 * @brief A folder to write configuration files in, and an interpreter with a
 * port and a nickname declared.
 */
class ConfigurationReaderFixture : public ::testing::Test {
 protected:
  ConfigurationReaderFixture();

  /** @brief Writes @p content as the configuration file of the tests. */
  void writeFile(std::string_view content) const;

  /** @returns The path of the configuration file of the tests. */
  [[nodiscard]] std::filesystem::path file() const;

  /** @returns The folder the files of the tests are written in. */
  [[nodiscard]] const TemporaryFolder& folder() const;

  /** @returns The interpreter the `set` command is registered in. */
  [[nodiscard]] rtype::engine::CommandInterpreter& interpreter();

  /** @returns The port setting. */
  [[nodiscard]] const rtype::engine::SettingValue& port() const;

  /** @returns The nickname setting. */
  [[nodiscard]] const rtype::engine::SettingValue& nickname() const;

  /** @returns The report of reading @p path then running @p forcedLines. */
  [[nodiscard]] rtype::engine::ConfigurationReport read(
      const std::filesystem::path& path,
      const std::vector<std::string>& forcedLines);

  /** @returns The report of reading @p source with the interpreter of the
   * tests. */
  [[nodiscard]] rtype::engine::ConfigurationReport read(
      const rtype::engine::ConfigurationSource& source);

 private:
  TemporaryFolder folder_;
  rtype::engine::CommandInterpreter interpreter_;
  rtype::engine::ConfigurationRegistry registry_{interpreter_};
  const rtype::engine::SettingValue* port_;
  const rtype::engine::SettingValue* nickname_;
};
