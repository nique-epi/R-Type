#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>
#include "Application.hpp"
#include "GameStatus.hpp"
#include "IncomingDatagram.hpp"
#include "RecordingNetworkModule.hpp"
#include "RecordingProcessLauncher.hpp"
#include "ServerModuleFixture.hpp"
#include "TimeConstants.hpp"

/** @brief The base of the tests of the supervisor modules: the logger module, a
 * network module that records, the supervised games module on a launcher that
 * records, and the supervisor lobby after it, all started. */
class ServerSupervisedGamesFixture : public ServerModuleFixture {
 protected:
  /** @brief Builds the modules listed above and starts them. */
  void startSupervisor();

  /** @brief Stops the application, which stops every game. */
  void stopSupervisor();

  /** @brief Runs one tick with @p players and @p requests among the
   * datagrams. */
  void runTick(
      const std::vector<rtype::network::IncomingDatagram>& players,
      const std::vector<rtype::network::IncomingDatagram>& requests = {});

  /** @brief Runs simulation ticks until @p duration has passed. */
  void runFor(rtype::engine::Duration duration);

  /** @brief Has the player at @p index ask for a place with a valid cookie: a
   * first request, the challenge it gets, then the request with that cookie, in
   * two ticks. */
  void askForAPlaceAs(std::size_t index);

  /** @brief Has game @p game (by launch order) send @p status under
   * @p sequence, in one tick. */
  void gameReports(std::size_t game, const rtype::network::GameStatus& status,
                   std::uint16_t sequence);

  [[nodiscard]] RecordingProcessLauncher& launcher();
  [[nodiscard]] RecordingNetworkModule& network();

 private:
  std::unique_ptr<rtype::engine::Application> application_;
  RecordingNetworkModule* network_{nullptr};
  RecordingProcessLauncher* launcher_{nullptr};
};
