#pragma once
#include <gtest/gtest.h>
#include <cstdint>
#include <functional>
#include <memory>
#include "GameRegistry.hpp"
#include "GameSpawner.hpp"
#include "ListeningPortModule.hpp"
#include "RecordingPlayerAdmission.hpp"
#include "RecordingProcessLauncher.hpp"

/** @brief The base of the tests of the spawner: a launcher and an admission
 * that record, an empty registry, a supervisor on port 4242, and a spawner
 * whose free port is 50123 until a test says another. */
class GameSpawnerFixture : public ::testing::Test {
 protected:
  GameSpawnerFixture();

  /** @brief Replaces the spawner by one whose free port comes from
   * @p freePort. */
  void useFreePort(std::function<std::uint16_t()> freePort);

  [[nodiscard]] rtype::server::GameSpawner& spawner();
  [[nodiscard]] RecordingProcessLauncher& launcher();
  [[nodiscard]] rtype::server::GameRegistry& registry();
  [[nodiscard]] RecordingPlayerAdmission& admission();

 private:
  RecordingProcessLauncher launcher_;
  rtype::server::GameRegistry registry_;
  RecordingPlayerAdmission admission_;
  ListeningPortModule supervisorPort_;
  std::unique_ptr<rtype::server::GameSpawner> spawner_;
};
