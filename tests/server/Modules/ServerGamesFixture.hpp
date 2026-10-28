#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>
#include "Application.hpp"
#include "ConnectedPlayer.hpp"
#include "GamesSettings.hpp"
#include "IModule.hpp"
#include "IncomingDatagram.hpp"
#include "MessageType.hpp"
#include "RecordingNetworkModule.hpp"
#include "ServerModuleFixture.hpp"
#include "ServiceProbeModule.hpp"

/** @brief The base of the tests of the games module: the journal module, a
 * network module that records, the games module on the shipped waves file, and
 * a probe that finds the games, all started. */
class ServerGamesFixture : public ServerModuleFixture {
 protected:
  /** @brief Builds the modules listed above with @p settings and starts them.
   * @throws what Application::start() throws. */
  void startWith(rtype::server::GamesSettings settings);

  /** @returns The modules listed after the games module and before the
   * probe; none unless a derived fixture lists some. */
  [[nodiscard]] virtual std::vector<std::unique_ptr<rtype::engine::IModule>>
  modulesAfterTheGames() const;

  /** @returns The settings that read the waves file shipped in assets/. */
  [[nodiscard]] static rtype::server::GamesSettings shippedWaves();

  /** @brief Connects the player at @p index to the first game, under a token
   * of its own. */
  rtype::server::ConnectedPlayer& join(std::size_t index);

  /** @brief Runs one tick of the loop with @p players and @p requests among the
   * datagrams. */
  void runTick(
      const std::vector<rtype::network::IncomingDatagram>& players,
      const std::vector<rtype::network::IncomingDatagram>& requests = {});

  /** @returns A Disconnection from the player at @p index, as its client
   * sends it. */
  [[nodiscard]] static rtype::network::IncomingDatagram disconnectionFrom(
      std::size_t index);

  /** @returns How many datagrams of the type @p type went to the player at
   * @p index. */
  [[nodiscard]] std::size_t datagramsTo(std::size_t index,
                                        rtype::network::MessageType type);

  [[nodiscard]] static std::uint64_t tokenOf(std::size_t index);

  [[nodiscard]] rtype::engine::Application& application();
  [[nodiscard]] RecordingNetworkModule& network();
  [[nodiscard]] ServiceProbeModule& probe();

 private:
  std::unique_ptr<rtype::engine::Application> application_;
  RecordingNetworkModule* network_{nullptr};
  ServiceProbeModule* probe_{nullptr};
};
