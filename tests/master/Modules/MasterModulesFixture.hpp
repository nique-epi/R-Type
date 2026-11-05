#pragma once

#include <gtest/gtest.h>
#include <httplib.h>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include "Application.hpp"
#include "ConfigurationReport.hpp"
#include "IModule.hpp"
#include "JournalFile.hpp"
#include "LogLaunchOptions.hpp"
#include "TemporaryFolder.hpp"

/**
 * @brief The modules of the master on an empty folder and a free port, with a
 * journal of their own, and a client that calls the master.
 */
class MasterModulesFixture : public ::testing::Test {
 protected:
  MasterModulesFixture();

  /**
   * @returns An application of the modules of the master, then @p extra, not
   * started; it runs @p lines as the configuration, after the lines that give
   * it the database of the folder and the free port.
   */
  [[nodiscard]] std::unique_ptr<rtype::engine::Application> masterApplication(
      std::vector<std::unique_ptr<rtype::engine::IModule>> extra,
      std::vector<std::string> lines);

  /** @returns The line that gives the `database` setting @p file. */
  [[nodiscard]] static std::string databaseLine(
      const std::filesystem::path& file);

  /** @returns The answer of the master to GET @p path. */
  [[nodiscard]] httplib::Result get(std::string_view path) const;

  /** @brief Writes @p content in @p file, below folder(). */
  void writeFile(const std::filesystem::path& file,
                 std::string_view content) const;

  /** @returns The folder the database of the master lies in. */
  [[nodiscard]] const std::filesystem::path& folder() const;

  /** @returns The port the master listens on. */
  [[nodiscard]] std::uint16_t port() const;

 private:
  [[nodiscard]] rtype::logging::LogLaunchOptions journalOptions() const;

  JournalFile journal_;
  TemporaryFolder folder_;
  std::uint16_t port_{};
  rtype::engine::ConfigurationReport report_;
};
