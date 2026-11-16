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
#include "NetworkConstants.hpp"
#include "TemporaryFolder.hpp"

/**
 * @brief The modules of the master on an empty folder and a port the system
 * chooses, with a journal of their own, and a client that calls the master.
 */
class MasterModulesFixture : public ::testing::Test {
 protected:
  /**
   * @returns An application of the modules of the master, then a
   * HttpPortRecordingModule and @p extra, not started; it runs @p lines as the
   * configuration, after the lines that give it the database of the folder
   * and the port network::ANY_PORT.
   */
  [[nodiscard]] std::unique_ptr<rtype::engine::Application> masterApplication(
      std::vector<std::unique_ptr<rtype::engine::IModule>> extra,
      std::vector<std::string> lines);

  /** @returns The line that gives the `database` setting @p file. */
  [[nodiscard]] static std::string databaseLine(
      const std::filesystem::path& file);

  /** @returns The answer of the master to GET @p path. */
  [[nodiscard]] httplib::Result get(std::string_view path) const;

  /** @returns The folder the database of the master lies in. */
  [[nodiscard]] const std::filesystem::path& folder() const;

  /** @returns The port the master listens on once started. */
  [[nodiscard]] std::uint16_t port() const;

  /** @returns The lines of the journal of the master that hold @p text. */
  [[nodiscard]] std::vector<std::string> journalLinesHolding(
      std::string_view text) const;

 private:
  [[nodiscard]] rtype::logging::LogLaunchOptions journalOptions() const;

  JournalFile journal_;
  TemporaryFolder folder_;
  std::uint16_t port_{rtype::network::ANY_PORT};
  rtype::engine::ConfigurationReport report_;
};
