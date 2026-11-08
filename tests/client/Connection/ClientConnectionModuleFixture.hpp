#pragma once

#include <gtest/gtest.h>
#include <memory>
#include <string>
#include "Application.hpp"
#include "ConfigurationReport.hpp"
#include "Endpoint.hpp"
#include "IServerCatalog.hpp"
#include "JournalFile.hpp"
#include "RunningMaster.hpp"
#include "ServerListPhase.hpp"
#include "TemporaryFolder.hpp"

namespace rtype::client::testing {

/**
 * @brief The connection module of the client, after a logger and a stand-in
 * for the assets, read by a module that finds its server catalog, next to a
 * running master.
 */
class ClientConnectionModuleFixture : public ::testing::Test {
 protected:
  /** @brief Stops the client before the master it reads goes away. */
  void TearDown() override;

  /** @brief Builds and starts the client, its configuration forcing @p line.
   */
  void startWith(const std::string& line);

  /** @brief Asks the server catalog for the list of servers again. */
  void askListing();

  /** @returns @p awaited once the read of the list of servers shows it, the
   * stages run every LISTING_POLL_STEP; the phase it shows when
   * LISTING_DEADLINE passed first. */
  [[nodiscard]] ServerListPhase phaseSoon(ServerListPhase awaited);

  /** @returns The running master, as a player names it. */
  [[nodiscard]] network::Endpoint masterEndpoint() const;

  /** @returns The url of a port on this machine no master listens on. */
  [[nodiscard]] static std::string urlWithoutMaster();

 private:
  JournalFile journal_;
  TemporaryFolder folder_;
  RunningMaster master_;
  engine::ConfigurationReport report_;
  IServerCatalog* catalog_{nullptr};
  std::unique_ptr<engine::Application> application_;
};

}  // namespace rtype::client::testing
