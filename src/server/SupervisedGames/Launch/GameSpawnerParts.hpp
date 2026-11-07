#pragma once

#include <cstdint>
#include <functional>
#include "GameLaunchSettings.hpp"
#include "GameRegistry.hpp"
#include "IListeningPort.hpp"
#include "IPlayerAdmission.hpp"
#include "IProcessLauncher.hpp"
#include "SettingValue.hpp"

namespace rtype::server {

/** @brief What a GameSpawner is built from; everything it refers to must
 * outlive it. */
struct GameSpawnerParts {
  std::reference_wrapper<IProcessLauncher> launcher;
  std::reference_wrapper<GameRegistry> registry;
  std::reference_wrapper<IPlayerAdmission> admission;
  /** @brief The port the supervisor listens on, which the games talk to. */
  std::reference_wrapper<const IListeningPort> supervisorPort;
  std::reference_wrapper<const GameLaunchSettings> settings;
  /** @brief The database setting of the supervisor, read when a game starts:
   * the leaderboard every game writes to while it is not empty. */
  std::reference_wrapper<const engine::SettingValue> database;
  /** @brief Gives the port a new game listens on; freeUdpPort() in the
   * supervisor. */
  std::function<std::uint16_t()> freePort;
};

}  // namespace rtype::server
