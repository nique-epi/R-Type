#pragma once

#include <cstddef>
#include <functional>
#include "GameLoaded.hpp"
#include "MapVote.hpp"
#include "MessageOrigin.hpp"
#include "PlayerRoster.hpp"
#include "ReadyState.hpp"

namespace rtype::server {

/**
 * @brief Keeps whether each player declared itself ready, the map it votes
 * for, and whether it told the game it shows the game screen.
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
   * @param mapCount How many maps the waiting room offers to the vote.
   */
  ReadyKeeper(PlayerRoster& roster, std::function<bool()> readyAccepted,
              std::size_t mapCount);

  /**
   * @brief Records @p message for the player who sent it, when readyAccepted
   * says it counts and the sender is a player.
   */
  void apply(const network::MessageOrigin& origin,
             const network::ReadyState& message);

  /**
   * @brief Records the map the player who sent @p message votes for, when
   * readyAccepted says the waiting room takes its choices, the sender is a
   * player, and the map is NO_MAP_VOTE or one of the mapCount maps.
   */
  void apply(const network::MessageOrigin& origin,
             const network::MapVote& message);

  /**
   * @brief Records that the player who sent @p message is loaded, in any state
   * of the game; does nothing when the sender is not a player.
   */
  void apply(const network::MessageOrigin& origin,
             const network::GameLoaded& message);

  /**
   * @returns Whether at least one player is connected and every connected
   * player is ready.
   */
  [[nodiscard]] bool everyPlayerIsReady();

  /** @returns Whether the game takes the choices of the waiting room right
   * now: a ReadyState, a ShipLookChoice. */
  [[nodiscard]] bool acceptsRoomChoices() const;

 private:
  PlayerRoster& roster_;
  std::function<bool()> readyAccepted_;
  std::size_t mapCount_;
};

}  // namespace rtype::server
