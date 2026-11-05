#include "DatagramSorter.hpp"
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string_view>
#include <vector>
#include "AdmittedPlayer.hpp"
#include "AllowanceSpending.hpp"
#include "AllowanceVerdict.hpp"
#include "BoundedQueue.hpp"
#include "DatagramCodec.hpp"
#include "DatagramDrops.hpp"
#include "DatagramQueues.hpp"
#include "Endpoint.hpp"
#include "IClock.hpp"
#include "IncomingDatagram.hpp"
#include "MessageType.hpp"
#include "NetworkConstants.hpp"
#include "PacketHeader.hpp"
#include "ProtocolConstants.hpp"
#include "RateLimitConstants.hpp"
#include "SenderAllowance.hpp"
#include "ServerConstants.hpp"
#include "SortedQueue.hpp"
#include "SpokenDatagram.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

static_assert(
    network::PLAYER_DATAGRAMS_PER_SECOND >
        static_cast<std::int64_t>(engine::SIMULATION_TICKS_PER_SECOND),
    "a player sends an input every simulation tick, and heartbeats "
    "besides: its limit must be above the tick rate");

namespace {

/**
 * @returns Whether @p header carries @p sessionToken, or NO_SESSION_TOKEN in a
 * datagram that holds a message sent outside any session.
 */
bool carriesSessionToken(const network::PacketHeader& header,
                         std::uint64_t sessionToken, bool outsideSession) {
  return header.sessionToken == sessionToken ||
         (outsideSession && header.sessionToken == network::NO_SESSION_TOKEN);
}

/**
 * @returns The queue a datagram holding @p types goes to: the connection
 * requests when it holds a ConnectionRequest, whatever else it holds, then the
 * information requests, then the players.
 */
SortedQueue queueFor(const network::DatagramCodec::MessageTypes& types) {
  if (types.test(
          static_cast<std::size_t>(network::MessageType::ConnectionRequest))) {
    return SortedQueue::ConnectionRequests;
  }
  if (types.test(static_cast<std::size_t>(
          network::MessageType::ServerInformationRequest))) {
    return SortedQueue::InformationRequests;
  }
  return SortedQueue::Players;
}

/** @returns Whether @p datagram holds a message sent outside any session. */
bool isOutsideSession(const SpokenDatagram& datagram) {
  return datagram.queue != SortedQueue::Players;
}

/** @returns The queue of @p destinations that @p datagram goes to. */
concurrency::BoundedQueue<network::IncomingDatagram>& queueOf(
    const DatagramQueues& destinations, const SpokenDatagram& datagram) {
  switch (datagram.queue) {
    case SortedQueue::ConnectionRequests:
      return *destinations.connectionRequests;
    case SortedQueue::InformationRequests:
      return *destinations.informationRequests;
    case SortedQueue::Players:
      break;
  }
  return *destinations.players;
}

/**
 * @returns A copy of @p datagram from @p sender, stamped with @p receivedAt,
 * read on the clock of the sorter: the time it arrived, since the sorter runs
 * in the handler of the socket.
 */
network::IncomingDatagram copyOf(const network::Endpoint& sender,
                                 std::span<const std::byte> datagram,
                                 std::chrono::nanoseconds receivedAt) {
  return network::IncomingDatagram{
      .sender = sender,
      .payload = {datagram.begin(), datagram.end()},
      .receivedAt = receivedAt};
}

AdmittedPlayer freshPlayer(const network::Endpoint& player,
                           std::uint64_t sessionToken) {
  return AdmittedPlayer{.endpoint = player,
                        .sessionToken = sessionToken,
                        .allowance = network::SenderAllowance{}};
}

/** @returns Why a datagram given @p verdict is dropped, or nothing when it
 * goes through. */
std::optional<std::string_view> dropReasonOf(
    network::AllowanceVerdict verdict) {
  switch (verdict) {
    case network::AllowanceVerdict::Allowed:
      return std::nullopt;
    case network::AllowanceVerdict::OverCap:
      return OVER_RATE_LIMIT_DROP_REASON;
    case network::AllowanceVerdict::NewlyBanned:
      return NEWLY_BANNED_DROP_REASON;
    case network::AllowanceVerdict::Banned:
      return BANNED_DROP_REASON;
  }
  return std::nullopt;
}

/** @returns Whether @p verdict lets the datagram of @p sender through; drops
 * it in @p drops, with the reason, otherwise. */
bool keptByLimit(network::AllowanceVerdict verdict,
                 const network::Endpoint& sender, DatagramDrops& drops) {
  const std::optional<std::string_view> reason = dropReasonOf(verdict);
  if (reason.has_value()) {
    drops.drop(sender, *reason);
  }
  return !reason.has_value();
}

}  // namespace

DatagramSorter::DatagramSorter(DatagramQueues destinations,
                               DatagramDrops& drops,
                               const engine::IClock& clock)
    : destinations_(destinations), drops_(&drops), clock_(&clock) {}

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
  sortSpoken(SpokenDatagram{.sender = sender,
                            .header = header,
                            .queue = queueFor(*types),
                            .bytes = datagram});
}

void DatagramSorter::sortSpoken(const SpokenDatagram& datagram) {
  const network::Endpoint& sender = datagram.sender.get();
  AdmittedPlayer* const player = findAdmitted(sender);
  const bool outsideSession = isOutsideSession(datagram);
  if (player != nullptr &&
      !carriesSessionToken(datagram.header, player->sessionToken,
                           outsideSession)) {
    drops_->dropWrongToken(sender);
    return;
  }
  if (player == nullptr && !outsideSession) {
    drops_->drop(sender, STRANGER_DROP_REASON);
    return;
  }
  const std::chrono::nanoseconds now = clock_->now();
  if (keptByLimit(spendAllowanceOf(datagram, player, now), sender, *drops_)) {
    queueOf(destinations_, datagram).push(copyOf(sender, datagram.bytes, now));
  }
}

network::AllowanceVerdict DatagramSorter::spendAllowanceOf(
    const SpokenDatagram& datagram, AdmittedPlayer* player,
    std::chrono::nanoseconds now) {
  if (player != nullptr && !isOutsideSession(datagram)) {
    return network::spendAllowance(player->allowance,
                                   network::PLAYER_RATE_LIMIT, now);
  }
  return requests_.spend(datagram.sender.get(), now);
}

void DatagramSorter::sortForeignHeader(const network::Endpoint& sender,
                                       const network::PacketHeader& header,
                                       std::span<const std::byte> datagram) {
  if (!isRefusedForItsVersion(sender, header, datagram.size())) {
    drops_->drop(sender, network::FOREIGN_HEADER_DROP_REASON);
    return;
  }
  const std::chrono::nanoseconds now = clock_->now();
  if (keptByLimit(requests_.spend(sender, now), sender, *drops_)) {
    destinations_.connectionRequests->push(copyOf(sender, datagram, now));
  }
}

bool DatagramSorter::isRefusedForItsVersion(const network::Endpoint& sender,
                                            const network::PacketHeader& header,
                                            std::size_t datagramSize) {
  return network::DatagramCodec::speaksAnotherVersion(header) &&
         findAdmitted(sender) == nullptr &&
         datagramSize >= network::CONNECTION_REFUSED_DATAGRAM_SIZE;
}

void DatagramSorter::admit(const network::Endpoint& player,
                           std::uint64_t sessionToken) {
  AdmittedPlayer* const admitted = findAdmitted(player);
  if (admitted == nullptr) {
    admitted_.push_back(freshPlayer(player, sessionToken));
    return;
  }
  if (admitted->sessionToken != sessionToken) {
    *admitted = freshPlayer(player, sessionToken);
  }
}

bool DatagramSorter::forget(const network::Endpoint& player,
                            std::uint64_t sessionToken) {
  return std::erase_if(admitted_,
                       [&player, sessionToken](const AdmittedPlayer& admitted) {
                         return admitted.endpoint == player &&
                                admitted.sessionToken == sessionToken;
                       }) > 0;
}

AdmittedPlayer* DatagramSorter::findAdmitted(const network::Endpoint& sender) {
  const auto admitted =
      std::ranges::find(admitted_, sender, &AdmittedPlayer::endpoint);
  return admitted == admitted_.end() ? nullptr : &*admitted;
}

}  // namespace rtype::server
