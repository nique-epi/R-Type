#pragma once

#include <chrono>
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include "IncomingDatagram.hpp"
#include "LatestValue.hpp"
#include "MasterProbe.hpp"
#include "RequestReader.hpp"
#include "ServerConstants.hpp"
#include "ServerMasterConstants.hpp"

namespace rtype::server {

/**
 * @brief Keeps the token of the probes of the master among the datagrams the
 * server received, as the Probe of the master section of the protocol page
 * describes: only from a probe that carries the secret this server registered
 * with, so a forged probe never replaces the token of the master. It sends
 * nothing.
 *
 * A datagram is read by a RequestReader, its first MasterProbe only; one it
 * cannot read, or whose secret is another, is dropped and logged at the debug
 * level at most once per REJECTION_LINE_INTERVAL. Tokens carry no order: the
 * last one read wins, and one more than 15 s late costs one heartbeat without
 * a valid token. Not thread-safe; the tokens it publishes are read from any
 * thread.
 */
class MasterProbeDesk {
 public:
  /**
   * @param secret The probeSecret of the registration of this server.
   * @param tokens Where the tokens kept go; it must outlive the desk.
   */
  MasterProbeDesk(std::uint64_t secret,
                  concurrency::LatestValue<std::uint64_t>& tokens);

  /**
   * @brief Publishes the token of each probe of @p datagrams that carries the
   * secret, oldest first, so the newest one stays.
   * @param now On the clock that stamped IncomingDatagram::receivedAt.
   */
  void keep(std::span<const network::IncomingDatagram> datagrams,
            std::chrono::nanoseconds now);

 private:
  /** @returns The token of the probe @p datagram holds, nothing when it
   * cannot be read or carries another secret, which is then logged. */
  [[nodiscard]] std::optional<std::uint64_t> tokenOf(
      const network::IncomingDatagram& datagram, std::chrono::nanoseconds now);

  std::uint64_t secret_;
  concurrency::LatestValue<std::uint64_t>& tokens_;
  RequestReader<network::MasterProbe> probes_{std::string{SERVER_LOGGER_NAME},
                                              NO_MASTER_PROBE_DROP_REASON};
};

}  // namespace rtype::server
