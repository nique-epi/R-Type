#include "DatagramSorter.hpp"
#include <algorithm>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "BoundedQueue.hpp"
#include "DatagramCodec.hpp"
#include "Endpoint.hpp"
#include "IClock.hpp"
#include "IncomingDatagram.hpp"
#include "MessageType.hpp"
#include "NetworkConstants.hpp"
#include "PacketHeader.hpp"
#include "ProtocolConstants.hpp"
#include "ServerConstants.hpp"

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

/**
 * @returns A copy of @p datagram from @p sender, stamped with the time on
 * @p clock: the time it arrived, since the sorter runs in the handler of the
 * socket.
 */
network::IncomingDatagram copyOf(const network::Endpoint& sender,
                                 std::span<const std::byte> datagram,
                                 const engine::IClock& clock) {
  return network::IncomingDatagram{
      .sender = sender,
      .payload = {datagram.begin(), datagram.end()},
      .receivedAt = clock.now()};
}

}  // namespace

DatagramSorter::DatagramSorter(Destinations destinations,
                               const engine::IClock& clock)
    : destinations_(destinations),
      clock_(&clock),
      dropLog_(std::string{SERVER_LOGGER_NAME},
               network::REJECTION_LINE_INTERVAL) {}

void DatagramSorter::sort(const network::Endpoint& sender,
                          std::span<const std::byte> datagram) {
  if (datagram.size() < network::PACKET_HEADER_SIZE) {
    drop(sender, SHORTER_THAN_HEADER_DROP_REASON);
    return;
  }
  const network::PacketHeader header =
      network::DatagramCodec::decodeHeader(datagram);
  if (!network::DatagramCodec::speaksThisProtocol(header)) {
    sortForeignHeader(sender, header, datagram);
    return;
  }
  const std::optional<network::DatagramCodec::MessageTypes> types =
      network::DatagramCodec::messageTypes(datagram);
  if (!types.has_value()) {
    drop(sender, MESSAGE_SIZES_DROP_REASON);
    return;
  }
  const bool holdsRequest = types->test(
      static_cast<std::size_t>(network::MessageType::ConnectionRequest));
  const AdmittedPlayer* const player = findAdmitted(sender);
  if (player != nullptr &&
      !carriesSessionToken(header, player->sessionToken, holdsRequest)) {
    dropWrongToken(sender);
    return;
  }
  if (holdsRequest) {
    destinations_.connectionRequests->push(copyOf(sender, datagram, *clock_));
  } else if (player != nullptr) {
    destinations_.players->push(copyOf(sender, datagram, *clock_));
  } else {
    drop(sender, STRANGER_DROP_REASON);
  }
}

void DatagramSorter::sortForeignHeader(const network::Endpoint& sender,
                                       const network::PacketHeader& header,
                                       std::span<const std::byte> datagram) {
  if (isRefusedForItsVersion(sender, header, datagram.size())) {
    destinations_.connectionRequests->push(copyOf(sender, datagram, *clock_));
    return;
  }
  drop(sender, network::FOREIGN_HEADER_DROP_REASON);
}

bool DatagramSorter::isRefusedForItsVersion(const network::Endpoint& sender,
                                            const network::PacketHeader& header,
                                            std::size_t datagramSize) const {
  return network::DatagramCodec::speaksAnotherVersion(header) &&
         findAdmitted(sender) == nullptr &&
         datagramSize >= network::CONNECTION_REFUSED_DATAGRAM_SIZE;
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

bool DatagramSorter::forget(const network::Endpoint& player,
                            std::uint64_t sessionToken) {
  return std::erase_if(admitted_,
                       [&player, sessionToken](const AdmittedPlayer& admitted) {
                         return admitted.endpoint == player &&
                                admitted.sessionToken == sessionToken;
                       }) > 0;
}

std::size_t DatagramSorter::droppedCount() const {
  return droppedCount_.load(std::memory_order_relaxed);
}

std::size_t DatagramSorter::wrongTokenCount() const {
  return wrongTokenCount_.load(std::memory_order_relaxed);
}

std::optional<network::Endpoint> DatagramSorter::takeWrongTokenSender() {
  const std::scoped_lock lock{wrongTokenSenderMutex_};
  return std::exchange(wrongTokenSender_, std::nullopt);
}

const DatagramSorter::AdmittedPlayer* DatagramSorter::findAdmitted(
    const network::Endpoint& sender) const {
  const auto admitted =
      std::ranges::find(admitted_, sender, &AdmittedPlayer::endpoint);
  return admitted == admitted_.end() ? nullptr : &*admitted;
}

void DatagramSorter::drop(const network::Endpoint& sender,
                          std::string_view reason) {
  droppedCount_.fetch_add(1, std::memory_order_relaxed);
  dropLog_.debug(clock_->now(), "dropped a datagram from ", sender.address,
                 " port ", sender.port, ": ", reason);
}

void DatagramSorter::dropWrongToken(const network::Endpoint& sender) {
  dropLog_.debug(clock_->now(), "dropped a datagram from ", sender.address,
                 " port ", sender.port, ": ", WRONG_TOKEN_DROP_REASON);
  const std::scoped_lock lock{wrongTokenSenderMutex_};
  if (!wrongTokenSender_.has_value()) {
    wrongTokenSender_ = sender;
  }
  wrongTokenCount_.fetch_add(1, std::memory_order_relaxed);
}

}  // namespace rtype::server
