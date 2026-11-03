#include "TrafficCounters.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <span>
#include "DatagramCodec.hpp"
#include "DirectionTraffic.hpp"
#include "Endpoint.hpp"
#include "MessageFrame.hpp"
#include "MessageSlotConstants.hpp"
#include "PeerTraffic.hpp"
#include "ProtocolConstants.hpp"
#include "TrafficFigures.hpp"
#include "TrafficReport.hpp"
#include "TwoWayTraffic.hpp"

namespace rtype::network {

namespace {

void addOne(TrafficFigures& figures, std::size_t bytes) {
  addFigures(figures, TrafficFigures{.count = 1, .bytes = bytes});
}

/**
 * @returns Whether @p datagram is of this protocol and version and its sizes
 * add up to its end, so forEachMessage() hands over all its messages or none.
 * The header comes first: reading it takes a fixed time and turns most foreign
 * bytes away before messageTypes() walks them, and the size comes before it,
 * since decodeHeader() throws on a datagram too short for a header.
 */
bool holdsReadableMessages(std::span<const std::byte> datagram) {
  return datagram.size() >= PACKET_HEADER_SIZE &&
         DatagramCodec::speaksThisProtocol(
             DatagramCodec::decodeHeader(datagram)) &&
         DatagramCodec::messageTypes(datagram).has_value();
}

/** @returns The slot of DirectionTraffic::messages for @p typeByte. */
std::size_t slotOf(std::uint8_t typeByte) {
  return typeByte < MESSAGE_SLOT_COUNT ? typeByte : UNKNOWN_MESSAGE_SLOT;
}

void addDatagram(DirectionTraffic& traffic,
                 std::span<const std::byte> datagram) {
  addOne(traffic.datagrams, datagram.size());
  if (!holdsReadableMessages(datagram)) {
    addOne(traffic.unreadable, datagram.size());
    return;
  }
  static_cast<void>(DatagramCodec::forEachMessage(
      datagram, [&traffic](const MessageFrame& message) noexcept {
        addOne(traffic.messages[slotOf(message.type)],
               MESSAGE_PREFIX_SIZE + message.payload.size());
      }));
}

void addDirection(DirectionTraffic& total, const DirectionTraffic& part) {
  addFigures(total.datagrams, part.datagrams);
  addFigures(total.unreadable, part.unreadable);
  for (std::size_t slot = 0; slot < MESSAGE_SLOT_COUNT; ++slot) {
    addFigures(total.messages[slot], part.messages[slot]);
  }
}

}  // namespace

void TrafficCounters::countSent(const Endpoint& destination,
                                std::span<const std::byte> datagram) {
  const std::scoped_lock lock{mutex_};
  addDatagram(trafficOf(destination).sent, datagram);
}

void TrafficCounters::countReceived(const Endpoint& sender,
                                    std::span<const std::byte> datagram) {
  const std::scoped_lock lock{mutex_};
  addDatagram(trafficOf(sender).received, datagram);
}

void TrafficCounters::follow(const Endpoint& peer) {
  const std::scoped_lock lock{mutex_};
  if (std::ranges::find(counted_.peers, peer, &PeerTraffic::peer) ==
      counted_.peers.end()) {
    counted_.peers.push_back(PeerTraffic{.peer = peer, .traffic = {}});
  }
}

void TrafficCounters::unfollow(const Endpoint& peer) {
  const std::scoped_lock lock{mutex_};
  const auto followed =
      std::ranges::find(counted_.peers, peer, &PeerTraffic::peer);
  if (followed == counted_.peers.end()) {
    return;
  }
  addDirection(counted_.departed.sent, followed->traffic.sent);
  addDirection(counted_.departed.received, followed->traffic.received);
  counted_.peers.erase(followed);
}

TrafficReport TrafficCounters::report() const {
  const std::scoped_lock lock{mutex_};
  return counted_;
}

TwoWayTraffic& TrafficCounters::trafficOf(const Endpoint& peer) {
  const auto followed =
      std::ranges::find(counted_.peers, peer, &PeerTraffic::peer);
  return followed == counted_.peers.end() ? counted_.others : followed->traffic;
}

}  // namespace rtype::network
