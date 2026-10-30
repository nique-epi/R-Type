#pragma once

#include <functional>
#include "MessageOrigin.hpp"
#include "PlayerRoster.hpp"
#include "ReadyState.hpp"

namespace rtype::server {

/**
 * @brief Keeps whether each player declared itself ready.
 *
 * A ReadyState counts only while the game accepts it. DatagramReader
 * already refused the datagrams that are not newer than the newest accepted
 * from that player, so a late copy never undoes a newer one.
 *
 * Not thread-safe: use it from the simulation thread only.
 */
class ReadyKeeper {
 public:
  /**
   * @param roster The players; it must outlive the keeper.
   * @param readyAccepted Tells whether a ReadyState counts right now; it must
   * not be empty.
   */
  ReadyKeeper(PlayerRoster& roster, std::function<bool()> readyAccepted);

  /**
   * @brief Records @p message for the player who sent it, when readyAccepted
   * says it counts and the sender is a player.
   */
  void apply(const network::MessageOrigin& origin,
             const network::ReadyState& message);

  /**
   * @returns Whether at least one player is connected and every connected
   * player is ready.
   */
  [[nodiscard]] bool everyPlayerIsReady();

 private:
  PlayerRoster& roster_;
  std::function<bool()> readyAccepted_;
};

}  // namespace rtype::server
