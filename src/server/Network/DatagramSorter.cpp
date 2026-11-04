#include "DatagramSorter.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <functional>
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
  sortSpoken(SpokenDatagram{
      .sender = std::cref(sender),
      .header = header,
      .holdsRequest = types->test(
          static_cast<std::size_t>(network::MessageType::ConnectionRequest)),
      .bytes = datagram});
}

void DatagramSorter::sortSpoken(const SpokenDatagram& datagram) {
  const network::Endpoint& sender = datagram.sender.get();
  AdmittedPlayer* const player = findAdmitted(sender);
  if (player != nullptr &&
      !carriesSessionToken(datagram.header, player->sessionToken,
                           datagram.holdsRequest)) {
    drops_->dropWrongToken(sender);
    return;
  }
  if (player == nullptr && !datagram.holdsRequest) {
    drops_->drop(sender, STRANGER_DROP_REASON);
    return;
  }
  if (keptByLimit(spendAllowanceOf(datagram, player), sender)) {
    pushCopy(datagram.holdsRequest ? *destinations_.connectionRequests
                                   : *destinations_.players,
             sender, datagram.bytes);
  }
}

network::AllowanceVerdict DatagramSorter::spendAllowanceOf(
    const SpokenDatagram& datagram, AdmittedPlayer* player) {
  if (player != nullptr &&
      datagram.header.sessionToken == player->sessionToken) {
    return network::spendAllowance(player->allowance,
                                   network::PLAYER_RATE_LIMIT, clock_->now());
  }
  return requests_.spend(datagram.sender.get(), clock_->now());
}

bool DatagramSorter::keptByLimit(network::AllowanceVerdict verdict,
                                 const network::Endpoint& sender) {
  const std::optional<std::string_view> reason = dropReasonOf(verdict);
  if (reason.has_value()) {
    drops_->drop(sender, *reason);
  }
  return !reason.has_value();
}

void DatagramSorter::sortForeignHeader(const network::Endpoint& sender,
                                       const network::PacketHeader& header,
                                       std::span<const std::byte> datagram) {
  if (!isRefusedForItsVersion(sender, header, datagram.size())) {
    drops_->drop(sender, network::FOREIGN_HEADER_DROP_REASON);
    return;
  }
  if (keptByLimit(requests_.spend(sender, clock_->now()), sender)) {
    pushCopy(*destinations_.connectionRequests, sender, datagram);
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
