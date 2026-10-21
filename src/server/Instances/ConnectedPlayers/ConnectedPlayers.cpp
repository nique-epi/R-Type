#include "ConnectedPlayers.hpp"
#include <cstddef>
#include <iterator>
#include "ConnectedPlayer.hpp"

namespace rtype::server {

void ConnectedPlayers::append(ConnectedPlayer* player) {
  players_.at(count_) = player;
  ++count_;
}

ConnectedPlayers::const_iterator ConnectedPlayers::begin() const {
  return players_.cbegin();
}

ConnectedPlayers::const_iterator ConnectedPlayers::end() const {
  return std::next(players_.cbegin(), static_cast<std::ptrdiff_t>(count_));
}

std::size_t ConnectedPlayers::size() const { return count_; }

bool ConnectedPlayers::empty() const { return count_ == 0; }

}  // namespace rtype::server
