#include <gtest/gtest.h>
#include <future>
#include <string>
#include <thread>
#include "Endpoint.hpp"
#include "NetworkTestConstants.hpp"
#include "ServerApplication.hpp"
#include "ServerTestConstants.hpp"
#include "TimeConstants.hpp"

using rtype::network::Endpoint;
using rtype::server::ServerApplication;

namespace {

Endpoint loopbackOnAnyPort() {
  return Endpoint{.address = std::string{LOOPBACK_ADDRESS}, .port = ANY_PORT};
}

}  // namespace

/**
 * Given a server opened on port 0
 * When its address is asked
 * Then it names the port the system picked, not 0
 */
TEST(ServerApplication, ListensOnThePortTheSystemPicked) {
  const ServerApplication server{loopbackOnAnyPort()};

  EXPECT_NE(server.localEndpoint().port, ANY_PORT);
}

/**
 * Given a server running on its own thread for a few ticks
 * When stop() is called from the test thread
 * Then run() returns
 */
TEST(ServerApplication, RunReturnsWhenStoppedFromAnotherThread) {
  ServerApplication server{loopbackOnAnyPort()};
  std::promise<void> returned;
  const std::future<void> runReturned = returned.get_future();
  const std::jthread runner{[&server, &returned] {
    server.run();
    returned.set_value();
  }};
  std::this_thread::sleep_for(rtype::engine::SIMULATION_TICK_DURATION *
                              SERVER_RUN_TICKS);

  server.stop();

  EXPECT_EQ(runReturned.wait_for(THREAD_TIMEOUT), std::future_status::ready);
}
