#include <gtest/gtest.h>
#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include "Application.hpp"
#include "Endpoint.hpp"
#include "EngineException.hpp"
#include "LoopStages.hpp"
#include "ModuleConstants.hpp"
#include "NetworkConstants.hpp"
#include "NetworkContext.hpp"
#include "NetworkSimulatorTestConstants.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "PortProbeModule.hpp"
#include "ServerConstants.hpp"
#include "ServerDatagrams.hpp"
#include "ServerModuleTestConstants.hpp"
#include "ServerNetworkModuleFixture.hpp"
#include "ServiceProbeModule.hpp"
#include "StopReason.hpp"
#include "TimeConstants.hpp"
#include "UdpSocket.hpp"
#include "UdpTestClient.hpp"

using rtype::engine::Application;
using rtype::engine::LoopStages;
using rtype::engine::ModuleStartFailedException;
using rtype::network::Endpoint;
using rtype::server::SERVER_LOOP_STAGES;

/**
 * Given a port and a module listed after the network module that tries to bind
 * that port while it installs
 * When the application starts
 * Then the port was free during install(): the network module binds in start()
 */
TEST_F(ServerNetworkModuleFixture, BindsNothingWhileItInstalls) {
  const std::uint16_t port = freeUdpPort();
  auto portProbe = std::make_unique<PortProbeModule>(
      PORT_PROBE_MODULE,
      std::vector<std::string>{std::string{rtype::server::NETWORK_MODULE_NAME}},
      port);
  const PortProbeModule* probe = portProbe.get();
  Application application(LoopStages(SERVER_LOOP_STAGES),
                          networkModulesThen(port, std::move(portProbe)));

  application.start();

  EXPECT_TRUE(probe->portWasFree());
}

/**
 * Given a started application whose network module listens on a port
 * When another socket tries to bind that port, then the application stops and
 * another socket tries again
 * Then the first try fails and the second succeeds: stop() gives the port back
 */
TEST_F(ServerNetworkModuleFixture, ListensOnItsPortFromStartToStop) {
  const std::uint16_t port = freeUdpPort();
  Application application(
      LoopStages(SERVER_LOOP_STAGES),
      networkModulesThen(port, probeRunning([](rtype::engine::Duration) {})));
  application.start();

  const bool freeWhileStarted = portIsFree(port);
  application.stop();

  EXPECT_FALSE(freeWhileStarted);
  EXPECT_TRUE(portIsFree(port));
}

/**
 * Given a module listed after the network module
 * When the application installs
 * Then it found the sender, the admission and the datagrams of the tick
 */
TEST_F(ServerNetworkModuleFixture, ProvidesItsServicesToTheModulesAfterIt) {
  auto probeModule = probeRunning([](rtype::engine::Duration) {});
  const ServiceProbeModule* probe = probeModule.get();
  Application application(
      LoopStages(SERVER_LOOP_STAGES),
      networkModulesThen(freeUdpPort(), std::move(probeModule)));

  application.start();

  EXPECT_NE(probe->sender(), nullptr);
  EXPECT_NE(probe->admission(), nullptr);
  EXPECT_NE(probe->tickDatagrams(), nullptr);
}

/**
 * Given a module that counts the passes of the simulation stage
 * When the application runs for a few ticks
 * Then the stage ran several times, each with the duration of one tick
 */
TEST_F(ServerNetworkModuleFixture,
       RunsTheStagesOncePerTickWithTheTickDuration) {
  std::atomic<int> passes{0};
  std::atomic<std::int64_t> lastElapsed{0};
  Application application(
      LoopStages(SERVER_LOOP_STAGES),
      networkModulesThen(freeUdpPort(),
                         probeRunning(countingPassesIn(passes, lastElapsed))));
  application.start();

  const bool ranTwice = waitForPasses(passes, PASSES_TO_WAIT_FOR);
  application.stop();

  EXPECT_TRUE(ranTwice);
  EXPECT_EQ(lastElapsed, rtype::engine::SIMULATION_TICK_DURATION.count());
}

/**
 * Given a player admitted through the admission the module provides
 * When that player's client sends a datagram with its token
 * Then the work of a stage sees it among the datagrams of the tick
 */
TEST_F(ServerNetworkModuleFixture, HandsTheTickTheDatagramsOfAnAdmittedPlayer) {
  const std::uint16_t port = freeUdpPort();
  std::atomic<std::size_t> playerDatagrams{0};
  std::atomic<int> passes{0};
  ServiceProbeModule* probe = nullptr;
  auto probeModule = probeRunning(
      [&probe, &playerDatagrams, &passes](rtype::engine::Duration) {
        playerDatagrams += probe->tickDatagrams()->players().size();
        ++passes;
      });
  probe = probeModule.get();
  Application application(LoopStages(SERVER_LOOP_STAGES),
                          networkModulesThen(port, std::move(probeModule)));
  application.start();
  rtype::network::NetworkContext clientContext;
  rtype::network::UdpSocket client{clientContext, loopbackOnPort(0)};
  probe->admission()->admit(loopbackOnPort(client.localEndpoint().port),
                            ADMITTED_TOKEN);
  rtype::network::PacketHeader header;
  header.sessionToken = ADMITTED_TOKEN;

  client.send(loopbackOnPort(port), readyDatagram(header));
  clientContext.run();
  const int passesAfterSending = passes;
  const bool ranAgain = waitForPasses(passes, passesAfterSending + 2);
  application.stop();

  EXPECT_TRUE(ranAgain);
  EXPECT_EQ(playerDatagrams, 1U);
}

/**
 * Given a stage work that throws while the server runs
 * When the error stops the server
 * Then the stop request is told the server failed, with the error's text
 */
TEST_F(ServerNetworkModuleFixture, TellsTheStopRequestWhenTheServerFails) {
  Application application(
      LoopStages(SERVER_LOOP_STAGES),
      networkModulesThen(freeUdpPort(),
                         probeRunning([](rtype::engine::Duration) {
                           throw std::runtime_error(STAGE_FAILURE_TEXT);
                         })));
  application.start();

  const rtype::server::StopReason reason = stopRequest().wait();
  application.stop();

  EXPECT_TRUE(reason.failed);
  EXPECT_EQ(reason.text, STAGE_FAILURE_TEXT);
}

/**
 * Given a port another socket already holds on every interface, as the server
 * would bind it (Windows lets a socket bound to one address share the port with
 * one bound to every address, so the holder must take the same address)
 * When the application starts
 * Then start() fails naming the network module, and the other socket keeps its
 * port
 */
TEST_F(ServerNetworkModuleFixture, FailsToStartWhenThePortIsTaken) {
  rtype::network::NetworkContext holderContext;
  const rtype::network::UdpSocket holder{
      holderContext,
      Endpoint{.address = std::string{rtype::network::ANY_IPV4_ADDRESS},
               .port = rtype::network::ANY_PORT}};
  Application application(
      LoopStages(SERVER_LOOP_STAGES),
      networkModulesThen(holder.localEndpoint().port,
                         probeRunning([](rtype::engine::Duration) {})));

  EXPECT_THROW(application.start(), ModuleStartFailedException);
  EXPECT_FALSE(portIsFree(holder.localEndpoint().port));
}

/**
 * Given a server whose network module crosses a simulated network with a lag
 * of 200 ms, and a client on the loopback address
 * When a module queues a datagram for the client
 * Then the client receives it no sooner than 100 ms after it was queued
 */
TEST_F(ServerNetworkModuleFixture, DelaysWhatItSendsByHalfTheSimulatedLag) {
  auto probeModule = probeRunning([](rtype::engine::Duration) {});
  const ServiceProbeModule* probe = probeModule.get();
  Application application(LoopStages(SERVER_LOOP_STAGES),
                          networkModulesOver(freeUdpPort(), LAGGING_NETWORK,
                                             std::move(probeModule)));
  application.start();
  UdpTestClient client;
  const auto queuedAt = std::chrono::steady_clock::now();

  probe->sender()->queueDatagram(rtype::network::OutgoingDatagram{
      .destination = client.localEndpoint(), .payload = {QUEUED_BYTE}});
  const std::vector<ReceivedAt> received =
      client.waitForDatagrams(1, WAIT_FOR_ANSWER);
  application.stop();

  ASSERT_EQ(received.size(), 1U);
  EXPECT_GE(received.front().time - queuedAt, HALF_OF_THE_LAG);
}
