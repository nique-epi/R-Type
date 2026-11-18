#pragma once

#include <optional>
#include <string>
#include <vector>
#include "IModule.hpp"
#include "IOptionalDependencies.hpp"
#include "InstallContext.hpp"
#include "LeaderboardController.hpp"

namespace rtype::master {

/**
 * @brief The module of the global leaderboard: the results of the vanilla
 * games the servers report, `PUT /api/servers/me/results/{gameId}`, kept in
 * the table `game_results`, and the leaderboard read from them,
 * `GET /api/leaderboard`.
 *
 * install() adds the routes to the MasterHttpServer, and reads the
 * ServersService of the servers module, which tells the server of a key; the
 * routes read the database only once the HTTP module listens. When the module
 * wallet is listed, before it, a result kept for the first time credits the
 * accounts of its players through its IAccountWallet; without it, a result
 * pays nothing. The module runs no thread: start() and stop() do nothing.
 */
class LeaderboardModule final : public engine::IModule,
                                public engine::IOptionalDependencies {
 public:
  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  /** @returns The module wallet, which credits what a game pays. */
  [[nodiscard]] std::vector<std::string> optionalDependencies() const override;

  /** @brief Adds the routes of the results and of the leaderboard; opens
   * nothing. */
  void install(engine::InstallContext& context) override;

  void start() override;
  void stop() override;

 private:
  std::optional<LeaderboardController> controller_;
};

}  // namespace rtype::master
