#pragma once

#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include "Application.hpp"
#include "ConfigurationReport.hpp"
#include "ServerModuleFixture.hpp"
#include "StopRequest.hpp"
#include "TemporaryFolder.hpp"

/** @brief The base of the tests of a whole supervisor: its modules on a free
 * port, its server.cfg in a folder of the test, read with the configuration
 * step main() gives it, and the file it writes for its games next to the
 * executable, removed after the test. */
class SupervisorModulesFixture : public ServerModuleFixture {
 public:
  SupervisorModulesFixture();
  SupervisorModulesFixture(const SupervisorModulesFixture&) = delete;
  SupervisorModulesFixture& operator=(const SupervisorModulesFixture&) = delete;
  SupervisorModulesFixture(SupervisorModulesFixture&&) = delete;
  SupervisorModulesFixture& operator=(SupervisorModulesFixture&&) = delete;
  ~SupervisorModulesFixture() override;

 protected:
  /** @brief Writes @p lines, one per line, to the server.cfg of the test. */
  void writeConfiguration(const std::vector<std::string>& lines) const;

  /** @returns The supervisor launched with @p options after its port and its
   * journal, reading the server.cfg of the test. Start it once. */
  [[nodiscard]] std::unique_ptr<rtype::engine::Application> supervisor(
      const std::vector<std::string_view>& options);

  /** @returns What the configuration step of the supervisor did. */
  [[nodiscard]] const rtype::engine::ConfigurationReport& report() const;

  /** @returns What the file of the games holds; empty when it is missing. */
  [[nodiscard]] std::string gameConfigurationText() const;

 private:
  [[nodiscard]] std::filesystem::path gameConfigurationFile() const;

  std::uint16_t port_;
  std::string portText_;
  std::string journalText_;
  TemporaryFolder folder_;
  rtype::server::StopRequest stopRequest_;
  rtype::engine::ConfigurationReport report_;
};
