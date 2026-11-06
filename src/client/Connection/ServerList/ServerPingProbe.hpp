#pragma once

#include <cstdint>
#include <optional>
#include <vector>
#include "Endpoint.hpp"
#include "IClock.hpp"
#include "IRoutedLink.hpp"
#include "IServerPingProbe.hpp"
#include "IncomingDatagram.hpp"
#include "PingAnswer.hpp"
#include "ServerInformation.hpp"

namespace rtype::client {

/**
 * @brief The probe of the list of servers, on a route of the socket of the
 * player: it listens to the servers of the round only, so the datagrams of a
 * game never reach it, and a late answer of the round before is dropped with
 * the change of route (see IRoutedLink::listenToEach()).
 *
 * A server keeps nothing for a ServerInformationRequest, so a round reserves
 * no seat and no nickname. An answer counts only when it echoes the sentAt of
 * the round, comes from a server of the round that has not answered yet, and
 * gives a round trip (see informationRoundTripOf()): a forged or repeated
 * answer moves no ping. A datagram the probe cannot read is dropped.
 *
 * A route sends and reads on the thread of the frame loop: so does the probe.
 */
class ServerPingProbe final : public IServerPingProbe {
 public:
  /**
   * @param link The route it sends and reads through; it must outlive the
   * probe.
   * @param clock The steady clock the network thread stamps each datagram with
   * (see IncomingDatagram::receivedAt); it must outlive the probe.
   */
  ServerPingProbe(IRoutedLink& link, const engine::IClock& clock);

  void measure(std::vector<network::Endpoint> servers) override;
  [[nodiscard]] std::vector<PingAnswer> answers() override;

 private:
  /** @returns What @p datagram says, when it is an answer to this round from
   * a server that has not answered yet. */
  [[nodiscard]] std::optional<network::ServerInformation> informationIn(
      const network::IncomingDatagram& datagram) const;
  /** @returns The answer @p information gives, when it holds a round trip, and
   * marks its server answered. */
  [[nodiscard]] std::optional<PingAnswer> answerOf(
      const network::IncomingDatagram& datagram,
      const network::ServerInformation& information);

  IRoutedLink* link_;
  const engine::IClock* clock_;
  std::uint32_t sentAt_{};
  std::vector<network::Endpoint> unanswered_;
  std::vector<network::IncomingDatagram> received_;
};

}  // namespace rtype::client
