#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>
#include "Application.hpp"
#include "Endpoint.hpp"
#include "IModule.hpp"
#include "ServerModuleFixture.hpp"
#include "StageWork.hpp"
#include "StopRequest.hpp"

class ServiceProbeModule;
class PortProbeModule;

/** @brief The base of the tests of the network module: the journal and
 * network modules of the server on a free port, then the probe the test
 * gives. */
class ServerNetworkModuleFixture : public ServerModuleFixture {
 protected:
  /**
   * @returns The journal module, the network module on @p port, then
   * @p probe, which depends on the network module.
   */
  [[nodiscard]] std::vector<std::unique_ptr<rtype::engine::IModule>>
  networkModulesThen(std::uint16_t port,
                     std::unique_ptr<rtype::engine::IModule> probe);

  /** @returns A probe listed after the network module that runs @p work in the
   * simulation stage; the test keeps it to read what it found. */
  [[nodiscard]] static std::unique_ptr<ServiceProbeModule> probeRunning(
      rtype::engine::StageWork work);

  /** @returns Whether a UDP socket can be bound on @p port now. */
  [[nodiscard]] static bool portIsFree(std::uint16_t port);

  /** @returns A work that counts its passes in @p passes and keeps the time of
   * the last one in @p lastElapsedNanoseconds. */
  [[nodiscard]] static rtype::engine::StageWork countingPassesIn(
      std::atomic<int>& passes,
      std::atomic<std::int64_t>& lastElapsedNanoseconds);

  /** @brief Waits until @p passes reaches @p wanted, or WAIT_FOR_THREADS.
   * @returns Whether it did. */
  [[nodiscard]] static bool waitForPasses(const std::atomic<int>& passes,
                                          int wanted);

  rtype::server::StopRequest stopRequest_;
};
