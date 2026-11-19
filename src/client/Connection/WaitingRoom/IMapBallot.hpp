#pragma once

#include <cstdint>
#include <optional>

namespace rtype::client {

/**
 * @brief The vote of the player for a map of the waiting room, as the screens
 * drive it. The maps and the votes the room shows are read from IWaitingRoom.
 */
class IMapBallot {
 public:
  virtual ~IMapBallot() = default;

  /**
   * @brief Asks the server to count the vote of the player for the map
   * numbered @p map in IWaitingRoom::lobbyMaps(), from 1, or for none with
   * NO_MAP_VOTE: a value, sent again until the waiting room shows it.
   * Ignored without a session, or once the game started.
   */
  virtual void voteFor(std::uint8_t map) = 0;

  /** @returns The map the player last asked to vote for in this session;
   * nothing before any request. */
  [[nodiscard]] virtual std::optional<std::uint8_t> requestedMap() const = 0;
};

}  // namespace rtype::client
