#include "PlayerRoster.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <optional>
#include <string>
#include <utility>
#include "ConnectedPlayer.hpp"
#include "Endpoint.hpp"
#include "ProtocolConstants.hpp"

namespace rtype::server {

ConnectedPlayer* PlayerRoster::find(const network::Endpoint& endpoint) {
  for (std::optional<ConnectedPlayer>& seat : seats_) {
    if (seat.has_value() && seat->endpoint == endpoint) {
      return &seat.value();
    }
  }
  return nullptr;
}

bool PlayerRoster::isFull() const {
  return std::ranges::all_of(seats_,
                             [](const std::optional<ConnectedPlayer>& seat) {
                               return seat.has_value();
                             });
}

ConnectedPlayer* PlayerRoster::add(const network::Endpoint& endpoint,
                                   std::string nickname,
                                   std::uint64_t sessionToken) {
  if (ConnectedPlayer* const connected = find(endpoint)) {
    return connected;
  }
  const auto freeSeat = std::ranges::find_if(
      seats_, [](const std::optional<ConnectedPlayer>& seat) {
        return !seat.has_value();
      });
  if (freeSeat == seats_.end()) {
    return nullptr;
  }
  const auto seatIndex =
      static_cast<std::size_t>(std::distance(seats_.begin(), freeSeat));
  return &freeSeat->emplace(ConnectedPlayer{
      .playerNumber =
          static_cast<std::uint8_t>(network::FIRST_PLAYER_NUMBER + seatIndex),
      .nickname = std::move(nickname),
      .endpoint = endpoint,
      .sessionToken = sessionToken,
      .nextSequence = 0});
}

}  // namespace rtype::server
