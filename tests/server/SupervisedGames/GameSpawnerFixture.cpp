#include "GameSpawnerFixture.hpp"
#include <cstdint>
#include <functional>
#include <utility>
#include "GameSpawner.hpp"
#include "GameSpawnerParts.hpp"
#include "NetworkConstants.hpp"
#include "SupervisedGamesTestConstants.hpp"

GameSpawnerFixture::GameSpawnerFixture()
    : supervisorPort_(rtype::network::DEFAULT_SERVER_PORT) {
  useFreePort([] { return FIRST_GAME_PORT; });
}

void GameSpawnerFixture::useFreePort(std::function<std::uint16_t()> freePort) {
  spawner_.emplace(rtype::server::GameSpawnerParts{
      .launcher = launcher_,
      .registry = registry_,
      .admission = admission_,
      .supervisorPort = supervisorPort_,
      .settings = {.program = PROGRAM_FOR_TESTS, .folder = FOLDER_FOR_TESTS},
      .freePort = std::move(freePort)});
}

rtype::server::GameSpawner& GameSpawnerFixture::spawner() { return *spawner_; }

RecordingProcessLauncher& GameSpawnerFixture::launcher() { return launcher_; }

rtype::server::GameRegistry& GameSpawnerFixture::registry() {
  return registry_;
}

RecordingPlayerAdmission& GameSpawnerFixture::admission() { return admission_; }
