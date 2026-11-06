#include "DatagramSorter.hpp"
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "AdmittedPlayer.hpp"
#include "AllowanceSpending.hpp"
#include "AllowanceVerdict.hpp"
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
#include "RateLimitConstants.hpp"
#include "RejectionCause.hpp"
#include "RejectionLedger.hpp"
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
 * information requests, then the probes, then the players.
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
  if (types.test(static_cast<std::size_t>(network::MessageType::MasterProbe))) {
    return SortedQueue::Probes;
  }
  return SortedQueue::Players;
}

/** @returns Whether @p datagram holds a message sent outside any session. */
bool isOutsideSession(const SpokenDatagram& datagram) {
  return datagram.queue != SortedQueue::Players;
}

/** @brief A queue of the sorter, and the cause a datagram is counted for when
 * it makes that queue discard its oldest. */
struct Destination {
  concurrency::BoundedQueue<network::IncomingDatagram>* queue{};
  RejectionCause overflow{RejectionCause::PlayerQueueFull};
};

/** @returns The queue of @p destinations that @p queue names, with its
 * cause. */
Destination destinationOf(const DatagramQueues& destinations,
                          SortedQueue queue) {
  switch (queue) {
    case SortedQueue::ConnectionRequests:
      return Destination{.queue = destinations.connectionRequests,
                         .overflow = RejectionCause::RequestQueueFull};
    case SortedQueue::InformationRequests:
      return Destination{.queue = destinations.informationRequests,
                         .overflow = RejectionCause::InformationQueueFull};
    case SortedQueue::Probes:
      return Destination{.queue = destinations.probes,
                         .overflow = RejectionCause::ProbeQueueFull};
    case SortedQueue::Players:
      break;
  }
  return Destination{.queue = destinations.players,
                     .overflow = RejectionCause::PlayerQueueFull};
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

/** @brief Why a datagram is dropped for its allowance. */
struct LimitDrop {
  RejectionCause cause;
  std::string_view reason;
};

/** @returns Why a datagram given @p verdict is dropped, or nothing when it
 * goes through. */
std::optional<LimitDrop> limitDropOf(network::AllowanceVerdict verdict) {
  switch (verdict) {
    case network::AllowanceVerdict::Allowed:
      return std::nullopt;
    case network::AllowanceVerdict::OverCap:
      return LimitDrop{.cause = RejectionCause::OverRateLimit,
                       .reason = OVER_RATE_LIMIT_DROP_REASON};
    case network::AllowanceVerdict::NewlyBanned:
      return LimitDrop{.cause = RejectionCause::NewlyBanned,
                       .reason = NEWLY_BANNED_DROP_REASON};
    case network::AllowanceVerdict::Banned:
      return LimitDrop{.cause = RejectionCause::Banned,
                       .reason = BANNED_DROP_REASON};
  }
  return std::nullopt;
}

/** @returns Whether the ConnectionAcceptor may refuse a datagram of
 * @p datagramSize bytes as IncompatibleVersion: its header is of another
 * version, and it holds at least CONNECTION_REFUSED_DATAGRAM_SIZE bytes, so the
 * refusal is never larger. A sender that was admitted gets no refusal. */
bool isRefusableForItsVersion(const network::PacketHeader& header,
                              std::size_t datagramSize) {
  return network::DatagramCodec::speaksAnotherVersion(header) &&
         datagramSize >= network::CONNECTION_REFUSED_DATAGRAM_SIZE;
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
    drop(sender, RejectionCause::WrongSessionToken, WRONG_TOKEN_DROP_REASON);
    return;
  }
  if (player == nullptr && !outsideSession) {
    drop(sender, RejectionCause::Stranger, STRANGER_DROP_REASON);
    return;
  }
  const std::chrono::nanoseconds now = clock_->now();
  if (keptByLimit(spendAllowanceOf(datagram, player, now), sender)) {
    enqueue(datagram.queue, sender, copyOf(sender, datagram.bytes, now));
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
  if (findAdmitted(sender) != nullptr ||
      !isRefusableForItsVersion(header, datagram.size())) {
    drop(sender, RejectionCause::ForeignHeader,
         network::FOREIGN_HEADER_DROP_REASON);
    return;
  }
  const std::chrono::nanoseconds now = clock_->now();
  if (keptByLimit(requests_.spend(sender, now), sender)) {
    enqueue(SortedQueue::ConnectionRequests, sender,
            copyOf(sender, datagram, now));
  }
}

void DatagramSorter::enqueue(SortedQueue queue, const network::Endpoint& sender,
                             network::IncomingDatagram copy) {
  const Destination destination = destinationOf(destinations_, queue);
  if (destination.queue->push(std::move(copy))) {
    rejections_->record(destination.overflow, sender);
  }
}

bool DatagramSorter::keptByLimit(network::AllowanceVerdict verdict,
                                 const network::Endpoint& sender) {
  const std::optional<LimitDrop> limitDrop = limitDropOf(verdict);
  if (limitDrop.has_value()) {
    drop(sender, limitDrop->cause, limitDrop->reason);
  }
  return !limitDrop.has_value();
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

void DatagramSorter::drop(const network::Endpoint& sender, RejectionCause cause,
                          std::string_view reason) {
  rejections_->record(cause, sender);
  dropLog_.debug(clock_->now(), "dropped a datagram from ", sender.address,
                 " port ", sender.port, ": ", reason);
}

}  // namespace rtype::server
