#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <vector>
#include "CreatedServer.hpp"
#include "MasterClient.hpp"
#include "MasterProbe.hpp"
#include "RunningMaster.hpp"
#include "UdpTestClient.hpp"

/**
 * @brief A running master, and a game server played by a UDP client on the
 * loopback: the server is created, and registers and beats through a
 * MasterClient holding its key.
 */
class MasterProbeModuleFixture : public ::testing::Test {
 protected:
  MasterProbeModuleFixture();

  /** @brief Registers the server as parisRegistration(), at the loopback
   * address and the port of the client. */
  void registerClient();

  /** @brief Sends a heartbeat of the server, without token. */
  void sendHeartbeat();

  /**
   * @returns The probes the client received once @p count arrived, or once
   * PROBE_DEADLINE passed, oldest first.
   */
  [[nodiscard]] std::vector<rtype::network::MasterProbe> probesSoon(
      std::size_t count);

 private:
  RunningMaster master_;
  UdpTestClient client_;
  rtype::network::CreatedServer created_;
  rtype::network::MasterClient server_;
};
