#include "DatagramSorter.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include "AdmittedPlayer.hpp"
#include "BoundedQueue.hpp"
#include "DatagramCodec.hpp"
#include "DatagramQueues.hpp"
#include "Endpoint.hpp"
#include "IClock.hpp"
#include "IncomingDatagram.hpp"
#include "MessageType.hpp"
#include "NetworkConstants.hpp"
#include "PacketHeader.hpp"
#include "ProtocolConstants.hpp"
#include "RejectionCause.hpp"
#include "RejectionLedger.hpp"
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
 * @returns Whether @p queue discarded its oldest datagram to make room for the
 * copy of @p datagram.
 */
bool pushCopy(concurrency::BoundedQueue<network::IncomingDatagram>& queue,
              const network::Endpoint& sender,
              std::span<const std::byte> datagram) {
  return queue.push(network::IncomingDatagram{
      .sender = sender, .payload = {datagram.begin(), datagram.end()}});
}

}  // namespace

DatagramSorter::DatagramSorter(DatagramQueues destinations,
                               RejectionLedger& rejections,
                               const engine::IClock& clock)
    : destinations_(destinations),
      rejections_(&rejections),
      clock_(&clock),
      dropLog_(std::string{SERVER_LOGGER_NAME},
               network::REJECTION_LINE_INTERVAL) {}

void DatagramSorter::sort(const network::Endpoint& sender,
                          std::span<const std::byte> datagram) {
  if (datagram.size() < network::PACKET_HEADER_SIZE) {
    drop(sender, RejectionCause::ShorterThanHeader,
         SHORTER_THAN_HEADER_DROP_REASON);
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
    drop(sender, RejectionCause::MessageSizes, MESSAGE_SIZES_DROP_REASON);
    return;
  }
  const bool holdsRequest = types->test(
      static_cast<std::size_t>(network::MessageType::ConnectionRequest));
  const AdmittedPlayer* const player = findAdmitted(sender);
  if (player != nullptr &&
      !carriesSessionToken(header, player->sessionToken, holdsRequest)) {
    drop(sender, RejectionCause::WrongSessionToken, WRONG_TOKEN_DROP_REASON);
    return;
  }
  if (holdsRequest) {
    queueRequest(sender, datagram);
  } else if (player != nullptr) {
    queuePlayerDatagram(sender, datagram);
  } else {
    drop(sender, RejectionCause::Stranger, STRANGER_DROP_REASON);
  }
}

void DatagramSorter::sortForeignHeader(const network::Endpoint& sender,
                                       const network::PacketHeader& header,
                                       std::span<const std::byte> datagram) {
  if (isRefusedForItsVersion(sender, header, datagram.size())) {
    queueRequest(sender, datagram);
    return;
  }
  drop(sender, RejectionCause::ForeignHeader,
       network::FOREIGN_HEADER_DROP_REASON);
}

void DatagramSorter::queueRequest(const network::Endpoint& sender,
                                  std::span<const std::byte> datagram) {
  if (pushCopy(*destinations_.connectionRequests, sender, datagram)) {
    rejections_->record(RejectionCause::RequestQueueFull, sender);
  }
}

void DatagramSorter::queuePlayerDatagram(const network::Endpoint& sender,
                                         std::span<const std::byte> datagram) {
  if (pushCopy(*destinations_.players, sender, datagram)) {
    rejections_->record(RejectionCause::PlayerQueueFull, sender);
  }
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

const AdmittedPlayer* DatagramSorter::findAdmitted(
    const network::Endpoint& sender) const {
  const auto admitted =
      std::ranges::find(admitted_, sender, &AdmittedPlayer::endpoint);
  return admitted == admitted_.end() ? nullptr : &*admitted;
}

void DatagramSorter::drop(const network::Endpoint& sender, RejectionCause cause,
                          std::string_view reason) {
  rejections_->record(cause, sender);
  dropLog_.debug(clock_->now(), "dropped a datagram from ", sender.address,
                 " port ", sender.port, ": ", reason);
}

}  // namespace rtype::server
