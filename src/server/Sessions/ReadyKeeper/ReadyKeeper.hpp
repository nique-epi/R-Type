#pragma once

#include <functional>
#include "MessageOrigin.hpp"
#include "PlayerRoster.hpp"
#include "ReadyState.hpp"

namespace rtype::server {

/**
 * @brief Keeps whether each player declared itself ready.
 *
 * A ReadyState counts only while the game accepts it, and only when its
 * sequence is newer than the newest one accepted from that player (see
 * network::isNewerSequence), so a late copy never undoes a newer one.
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
   * says it counts, the sender is a player and its sequence is newer.
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
