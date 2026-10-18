#include "DatagramSorter.hpp"
#include <algorithm>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include "BoundedQueue.hpp"
#include "DatagramCodec.hpp"
#include "Endpoint.hpp"
#include "IncomingDatagram.hpp"
#include "MessageType.hpp"
#include "PacketHeader.hpp"
#include "ProtocolConstants.hpp"

namespace rtype::server {

namespace {

/**
 * @returns Whether @p header carries @p sessionToken, or NO_SESSION_TOKEN in a
 * datagram holding a ConnectionRequest.
 */
bool carriesSessionToken(const network::PacketHeader& header,
                         std::uint64_t sessionToken, bool holdsRequest) {
  return header.sessionToken == sessionToken ||
         (holdsRequest && header.sessionToken == network::NO_SESSION_TOKEN);
}

void pushCopy(concurrency::BoundedQueue<network::IncomingDatagram>& queue,
              const network::Endpoint& sender,
              std::span<const std::byte> datagram) {
  queue.push(network::IncomingDatagram{
      .sender = sender, .payload = {datagram.begin(), datagram.end()}});
}

}  // namespace

DatagramSorter::DatagramSorter(Destinations destinations)
    : destinations_(destinations) {}

void DatagramSorter::sort(const network::Endpoint& sender,
                          std::span<const std::byte> datagram) {
  if (datagram.size() < network::PACKET_HEADER_SIZE) {
    drop();
    return;
  }
  const network::PacketHeader header =
      network::DatagramCodec::decodeHeader(datagram);
  if (!network::DatagramCodec::speaksThisProtocol(header)) {
    drop();
    return;
  }
  const std::optional<network::DatagramCodec::MessageTypes> types =
      network::DatagramCodec::messageTypes(datagram);
  if (!types.has_value()) {
    drop();
    return;
  }
  const bool holdsRequest = types->test(
      static_cast<std::size_t>(network::MessageType::ConnectionRequest));
  const AdmittedPlayer* const player = findAdmitted(sender);
  if (player != nullptr &&
      !carriesSessionToken(header, player->sessionToken, holdsRequest)) {
    wrongTokenCount_.fetch_add(1, std::memory_order_relaxed);
    return;
  }
  if (holdsRequest) {
    pushCopy(*destinations_.connectionRequests, sender, datagram);
  } else if (player != nullptr) {
    pushCopy(*destinations_.players, sender, datagram);
  } else {
    drop();
  }
}

void DatagramSorter::admit(const network::Endpoint& player,
                           std::uint64_t sessionToken) {
  const auto admitted =
      std::ranges::find(admitted_, player, &AdmittedPlayer::endpoint);
  if (admitted != admitted_.end()) {
    admitted->sessionToken = sessionToken;
    return;
  }
  admitted_.push_back(
      AdmittedPlayer{.endpoint = player, .sessionToken = sessionToken});
}

std::size_t DatagramSorter::droppedCount() const {
  return droppedCount_.load(std::memory_order_relaxed);
}

std::size_t DatagramSorter::wrongTokenCount() const {
  return wrongTokenCount_.load(std::memory_order_relaxed);
}

const DatagramSorter::AdmittedPlayer* DatagramSorter::findAdmitted(
    const network::Endpoint& sender) const {
  const auto admitted =
      std::ranges::find(admitted_, sender, &AdmittedPlayer::endpoint);
  return admitted == admitted_.end() ? nullptr : &*admitted;
}

void DatagramSorter::drop() {
  droppedCount_.fetch_add(1, std::memory_order_relaxed);
}

}  // namespace rtype::server
