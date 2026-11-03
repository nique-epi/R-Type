#pragma once

#include <gtest/gtest.h>
#include <filesystem>
#include <memory>
#include "InstallContext.hpp"
#include "LoopStages.hpp"
#include "ServiceRegistry.hpp"
#include "SimulatedClock.hpp"
#include "TemporaryFolder.hpp"
#include "TrafficCounters.hpp"

/** @brief The services the metrics module is installed over: the traffic, the
 * loop stages of the client, a clock the test moves, and a folder for the file.
 */
class ClientMetricsModuleFixture : public ::testing::Test {
 protected:
  ClientMetricsModuleFixture();

  /** @returns The context of the module over the services. */
  [[nodiscard]] rtype::engine::InstallContext context();

  /** @returns The context of the module over a registry that holds no service.
   */
  [[nodiscard]] rtype::engine::InstallContext contextWithoutServices();

  /** @brief Moves the clock by a second, then runs one frame. */
  void runAFrameASecondLater();

  [[nodiscard]] rtype::network::TrafficCounters& traffic();
  [[nodiscard]] const std::shared_ptr<SimulatedClock>& clock() const;
  [[nodiscard]] std::filesystem::path trafficFile() const;
  [[nodiscard]] const std::filesystem::path& folder() const;

 private:
  TemporaryFolder folder_;
  std::shared_ptr<SimulatedClock> clock_{std::make_shared<SimulatedClock>()};
  rtype::network::TrafficCounters traffic_;
  rtype::engine::ServiceRegistry services_;
  rtype::engine::ServiceRegistry noServices_;
  rtype::engine::LoopStages stages_;
};
