#include "MasterProbeDesk.hpp"
#include <chrono>
#include <cstdint>
#include <optional>
#include <span>
#include "IncomingDatagram.hpp"
#include "LatestValue.hpp"
#include "MasterProbe.hpp"
#include "ServerMasterConstants.hpp"

namespace rtype::server {

MasterProbeDesk::MasterProbeDesk(
    std::uint64_t secret, concurrency::LatestValue<std::uint64_t>& tokens)
    : secret_(secret), tokens_(tokens) {}

void MasterProbeDesk::keep(std::span<const network::IncomingDatagram> datagrams,
                           std::chrono::nanoseconds now) {
  for (const network::IncomingDatagram& datagram : datagrams) {
    if (const std::optional<std::uint64_t> token = tokenOf(datagram, now)) {
      tokens_.publish(*token);
    }
  }
}

std::optional<std::uint64_t> MasterProbeDesk::tokenOf(
    const network::IncomingDatagram& datagram, std::chrono::nanoseconds now) {
  const std::optional<network::MasterProbe> probe = probes_.read(datagram, now);
  if (!probe.has_value()) {
    return std::nullopt;
  }
  if (probe->secret != secret_) {
    probes_.drop(datagram.sender, WRONG_PROBE_SECRET_DROP_REASON, now);
    return std::nullopt;
  }
  return probe->token;
}

}  // namespace rtype::server
