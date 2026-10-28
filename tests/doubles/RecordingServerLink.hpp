#pragma once

#include <cstdint>
#include <vector>
#include "Endpoint.hpp"
#include "IServerLink.hpp"
#include "OutgoingDatagram.hpp"
#include "RecordedSession.hpp"

/**
 * @brief IServerLink double that keeps what the modules ask of the network.
 *
 * Like the server, it tells apart what the ticks sent from what was sent once
 * the loop stopped: sendAtEndOfTick(), which the test calls at the end of each
 * tick as the network thread sends there, takes the datagrams queued during
 * the tick; sendQueuedAfterRun() moves the datagrams queued since then into
 * sentAfterRun(). queued() keeps every datagram ever queued, in order.
 */
class RecordingServerLink : public rtype::server::IServerLink {
 public:
  void send(rtype::network::OutgoingDatagram datagram) override;
  void admit(const rtype::network::Endpoint& player,
             std::uint64_t sessionToken) override;
  void forget(const rtype::network::Endpoint& player,
              std::uint64_t sessionToken) override;
  void sendQueuedAfterRun() override;

  /** @brief Takes what was queued since the previous tick, as the network
   * thread sends it at the end of each tick. */
  void sendAtEndOfTick();

  /** @returns Every datagram send() received, oldest first. */
  [[nodiscard]] const std::vector<rtype::network::OutgoingDatagram>& queued()
      const;
  /** @returns The datagrams sendQueuedAfterRun() sent, oldest first. */
  [[nodiscard]] const std::vector<rtype::network::OutgoingDatagram>&
  sentAfterRun() const;
  /** @returns The sessions admit() received, oldest first. */
  [[nodiscard]] const std::vector<RecordedSession>& admitted() const;
  /** @returns The sessions forget() received, oldest first. */
  [[nodiscard]] const std::vector<RecordedSession>& forgotten() const;

 private:
  std::vector<rtype::network::OutgoingDatagram> queued_;
  std::vector<rtype::network::OutgoingDatagram> waiting_;
  std::vector<rtype::network::OutgoingDatagram> sentAfterRun_;
  std::vector<RecordedSession> admitted_;
  std::vector<RecordedSession> forgotten_;
};
