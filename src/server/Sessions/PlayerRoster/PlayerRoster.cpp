#include "PlayerRoster.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "ConnectedPlayers.hpp"
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

ConnectedPlayer* PlayerRoster::findByNumber(std::uint8_t playerNumber) {
  for (std::optional<ConnectedPlayer>& seat : seats_) {
    if (seat.has_value() && seat->playerNumber == playerNumber) {
      return &seat.value();
    }
  }
  return nullptr;
}

ConnectedPlayers PlayerRoster::connected() {
  ConnectedPlayers players;
  for (std::optional<ConnectedPlayer>& seat : seats_) {
    if (seat.has_value()) {
      players.append(&seat.value());
    }
  }
  return players;
}

std::vector<network::Endpoint> PlayerRoster::endpoints() const {
  std::vector<network::Endpoint> players;
  players.reserve(occupiedSeatCount());
  for (const std::optional<ConnectedPlayer>& seat : seats_) {
    if (seat.has_value()) {
      players.push_back(seat->endpoint);
    }
  }
  return players;
}

std::size_t PlayerRoster::occupiedSeatCount() const {
  return static_cast<std::size_t>(std::ranges::count_if(
      seats_, [](const std::optional<ConnectedPlayer>& seat) {
        return seat.has_value();
      }));
}

bool PlayerRoster::hasNickname(std::string_view nickname) const {
  const auto lowered = [](char letter) {
    return letter >= 'A' && letter <= 'Z'
               ? static_cast<char>(letter - 'A' + 'a')
               : letter;
  };
  return std::ranges::any_of(
      seats_, [&](const std::optional<ConnectedPlayer>& seat) {
        return seat.has_value() && std::ranges::equal(seat->nickname, nickname,
                                                      {}, lowered, lowered);
      });
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
  for (std::size_t seatIndex = 0; seatIndex < seats_.size(); ++seatIndex) {
    std::optional<ConnectedPlayer>& seat = seats_.at(seatIndex);
    if (!seat.has_value()) {
      return &seat.emplace(
          ConnectedPlayer{.playerNumber = static_cast<std::uint8_t>(
                              network::FIRST_PLAYER_NUMBER + seatIndex),
                          .nickname = std::move(nickname),
                          .endpoint = endpoint,
                          .sessionToken = sessionToken,
                          .nextSequence = 0,
                          .newestReceivedSequence = std::nullopt,
                          .newestInput = std::nullopt});
    }
  }
  return nullptr;
}

std::optional<ConnectedPlayer> PlayerRoster::remove(
    const network::Endpoint& endpoint) {
  for (std::optional<ConnectedPlayer>& seat : seats_) {
    if (seat.has_value() && seat->endpoint == endpoint) {
      return std::exchange(seat, std::nullopt);
    }
  }
  return std::nullopt;
}

}  // namespace rtype::server
