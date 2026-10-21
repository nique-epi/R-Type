#pragma once

#include <array>
#include <cstddef>
#include "ConnectedPlayer.hpp"
#include "ProtocolConstants.hpp"

namespace rtype::server {

/**
 * @brief The players connected at one moment, in order of their number: at
 * most MAXIMUM_PLAYERS pointers held in place, so building the list allocates
 * nothing.
 *
 * The list is a copy. A player joining or leaving the roster afterwards leaves
 * it as it was, so a loop over it may remove from the roster the player it
 * visits; the pointer to a removed player must not be read after that.
 */
class ConnectedPlayers {
 public:
  using const_iterator =
      std::array<ConnectedPlayer*, network::MAXIMUM_PLAYERS>::const_iterator;

  /**
   * @brief Lists @p player after the players already listed.
   * @param player Never nullptr; the list holds MAXIMUM_PLAYERS players at
   * most.
   */
  void append(ConnectedPlayer* player);

  [[nodiscard]] const_iterator begin() const;
  [[nodiscard]] const_iterator end() const;
  [[nodiscard]] std::size_t size() const;
  [[nodiscard]] bool empty() const;

 private:
  std::array<ConnectedPlayer*, network::MAXIMUM_PLAYERS> players_{};
  std::size_t count_{0};
};

}  // namespace rtype::server
