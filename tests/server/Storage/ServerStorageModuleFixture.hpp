#pragma once

#include <cstdint>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "Application.hpp"
#include "ConfigurationReport.hpp"
#include "ServerModuleFixture.hpp"
#include "TemporaryFolder.hpp"

/** @brief The storage module on an empty folder, after the logger and, when a
 * test gives a port, a double of the network that gives it. */
class ServerStorageModuleFixture : public ServerModuleFixture {
 protected:
  /**
   * @returns An application of those modules, not started, that runs each of
   * @p lines as a command of the configuration before the modules start.
   */
  [[nodiscard]] std::unique_ptr<rtype::engine::Application> storageApplication(
      std::optional<std::uint16_t> port, std::vector<std::string> lines);

  /** @returns The line that gives the `database` setting @p file. */
  [[nodiscard]] static std::string databaseLine(
      const std::filesystem::path& file);

  /** @returns The folder the storage module puts the file of a port in. */
  [[nodiscard]] const std::filesystem::path& folder() const;

 private:
  TemporaryFolder folder_;
  rtype::engine::ConfigurationReport report_;
};
