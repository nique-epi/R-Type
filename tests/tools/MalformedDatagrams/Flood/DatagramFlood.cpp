#include "DatagramFlood.hpp"
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <thread>
#include <vector>
#include "FloodConstants.hpp"
#include "FloodRecipient.hpp"
#include "FloodReport.hpp"
#include "LoopbackPeer.hpp"
#include "MalformedDatagramConstants.hpp"
#include "PacketHeader.hpp"

namespace rtype::tools {

namespace {

/** @returns How many datagrams a flood of @p datagramsPerSecond sends in its
 * first @p elapsed. */
std::size_t dueCountAfter(std::chrono::steady_clock::duration elapsed,
                          std::size_t datagramsPerSecond) {
  return static_cast<std::size_t>(
      std::chrono::duration<double>(elapsed).count() *
      static_cast<double>(datagramsPerSecond));
}

}  // namespace

DatagramFlood::DatagramFlood(FloodRecipient recipient, std::uint32_t seed)
    : recipient_(recipient), builder_(seed) {}

FloodReport DatagramFlood::run(LoopbackPeer& peer) {
  const auto start = std::chrono::steady_clock::now();
  for (auto elapsed = std::chrono::steady_clock::duration::zero();
       elapsed < FLOOD_DURATION;
       elapsed = std::chrono::steady_clock::now() - start) {
    sendDue(peer, dueCountAfter(elapsed, recipient_.datagramsPerSecond));
    report_.receivedCount += peer.drainFrom(recipient_.port);
    std::this_thread::sleep_for(PAUSE_BETWEEN_SENDS);
  }
  return report_;
}

void DatagramFlood::sendDue(LoopbackPeer& peer, std::size_t dueCount) {
  if (dueCount > scheduledCount_ + FLOOD_CATCH_UP_LIMIT) {
    scheduledCount_ = dueCount - FLOOD_CATCH_UP_LIMIT;
  }
  for (; scheduledCount_ < dueCount; ++scheduledCount_) {
    sendNext(peer);
  }
}

void DatagramFlood::sendNext(LoopbackPeer& peer) {
  const std::size_t kind = sentCount_ % MALFORMED_DATAGRAM_KINDS.size();
  const std::vector<std::byte> datagram =
      builder_.build(MALFORMED_DATAGRAM_KINDS.at(kind), nextHeader());
  if (peer.send(recipient_.port, datagram)) {
    ++report_.sentByKind.at(kind);
  } else {
    ++report_.refusedBySystemCount;
  }
  ++sentCount_;
}

network::PacketHeader DatagramFlood::nextHeader() {
  ++sequence_;
  return network::PacketHeader{.sessionToken = recipient_.sessionToken,
                               .sequence = sequence_};
}

}  // namespace rtype::tools
