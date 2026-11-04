#include "DatagramSorter.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <vector>
#include "AdmittedPlayer.hpp"
#include "BoundedQueue.hpp"
#include "DatagramCodec.hpp"
#include "DatagramDrops.hpp"
#include "DatagramQueues.hpp"
#include "Endpoint.hpp"
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

void pushCopy(concurrency::BoundedQueue<network::IncomingDatagram>& queue,
              const network::Endpoint& sender,
              std::span<const std::byte> datagram) {
  queue.push(network::IncomingDatagram{
      .sender = sender, .payload = {datagram.begin(), datagram.end()}});
}

}  // namespace

DatagramSorter::DatagramSorter(DatagramQueues destinations,
                               DatagramDrops& drops)
    : destinations_(destinations), drops_(&drops) {}

void DatagramSorter::sort(const network::Endpoint& sender,
                          std::span<const std::byte> datagram) {
  if (datagram.size() < network::PACKET_HEADER_SIZE) {
    drops_->drop(sender, SHORTER_THAN_HEADER_DROP_REASON);
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
    drops_->drop(sender, MESSAGE_SIZES_DROP_REASON);
    return;
  }
  const bool holdsRequest = types->test(
      static_cast<std::size_t>(network::MessageType::ConnectionRequest));
  const AdmittedPlayer* const player = findAdmitted(sender);
  if (player != nullptr &&
      !carriesSessionToken(header, player->sessionToken, holdsRequest)) {
    drops_->dropWrongToken(sender);
    return;
  }
  if (holdsRequest) {
    pushCopy(*destinations_.connectionRequests, sender, datagram);
  } else if (player != nullptr) {
    pushCopy(*destinations_.players, sender, datagram);
  } else {
    drops_->drop(sender, STRANGER_DROP_REASON);
  }
}

void DatagramSorter::sortForeignHeader(const network::Endpoint& sender,
                                       const network::PacketHeader& header,
                                       std::span<const std::byte> datagram) {
  if (isRefusedForItsVersion(sender, header, datagram.size())) {
    pushCopy(*destinations_.connectionRequests, sender, datagram);
    return;
  }
  drops_->drop(sender, network::FOREIGN_HEADER_DROP_REASON);
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

}  // namespace rtype::server
