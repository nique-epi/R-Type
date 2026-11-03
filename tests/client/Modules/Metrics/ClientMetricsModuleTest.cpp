#include <gtest/gtest.h>
#include <cstddef>
#include <optional>
#include <string>
#include <vector>
#include "ClientMetricsModule.hpp"
#include "ClientMetricsModuleFixture.hpp"
#include "EngineException.hpp"
#include "InstallContext.hpp"
#include "ModuleConstants.hpp"
#include "NetworkConstants.hpp"
#include "NetworkException.hpp"
#include "TrafficCounters.hpp"
#include "TrafficFixtures.hpp"
#include "TrafficTestConstants.hpp"

using rtype::client::ClientMetricsModule;
using rtype::client::CONNECTION_MODULE_NAME;
using rtype::engine::InstallContext;
using rtype::engine::MissingServiceException;
using rtype::network::DEFAULT_SERVER_PORT;
using rtype::network::TrafficFileOpenException;
using rtype::network::testing::peerOnPort;
using rtype::network::testing::textOf;

/**
 * Given the metrics module
 * When it is asked what it depends on
 * Then it names the connection, which provides the traffic
 */
TEST_F(ClientMetricsModuleFixture, DependsOnTheConnection) {
  const ClientMetricsModule module{clock(), std::nullopt};

  EXPECT_EQ(module.dependencies(),
            (std::vector<std::string>{std::string{CONNECTION_MODULE_NAME}}));
}

/**
 * Given a registry without the traffic
 * When the module is installed
 * Then it refuses
 */
TEST_F(ClientMetricsModuleFixture, RefusesToInstallWithoutTheTraffic) {
  ClientMetricsModule module{clock(), std::nullopt};
  InstallContext installing = contextWithoutServices();

  EXPECT_THROW(module.install(installing), MissingServiceException);
}

/**
 * Given a started module with a file, and a datagram counted
 * When a frame runs a second later
 * Then the file holds its row
 */
TEST_F(ClientMetricsModuleFixture, WritesTheTrafficInTheFrameLoop) {
  ClientMetricsModule module{clock(), trafficFile()};
  InstallContext installing = context();
  module.install(installing);
  module.start();
  traffic().countSent(peerOnPort(DEFAULT_SERVER_PORT),
                      std::vector<std::byte>(1));

  runAFrameASecondLater();

  EXPECT_NE(textOf(trafficFile()).find(ONE_BYTE_SENT_ROW_END),
            std::string::npos);
  module.stop();
}

/**
 * Given a started module with a file, and a datagram counted
 * When it stops before a second passed
 * Then the file holds the last rows
 */
TEST_F(ClientMetricsModuleFixture, StoppingWritesTheLastRows) {
  ClientMetricsModule module{clock(), trafficFile()};
  InstallContext installing = context();
  module.install(installing);
  module.start();
  traffic().countSent(peerOnPort(DEFAULT_SERVER_PORT),
                      std::vector<std::byte>(1));

  module.stop();

  EXPECT_NE(textOf(trafficFile()).find(ONE_BYTE_SENT_ROW_END),
            std::string::npos);
}

/**
 * Given a module told to write to a folder
 * When it starts
 * Then it refuses
 */
TEST_F(ClientMetricsModuleFixture, RefusesAFileItCannotOpen) {
  ClientMetricsModule module{clock(), folder()};
  InstallContext installing = context();
  module.install(installing);

  EXPECT_THROW(module.start(), TrafficFileOpenException);
}
