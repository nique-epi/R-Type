#pragma once

#include <cstdint>
#include <optional>
#include "IMapBallot.hpp"
#include "ISessionObserver.hpp"
#include "ISessionOutbox.hpp"
#include "IWaitingRoom.hpp"
#include "LobbyState.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The part of the session that carries the map vote of the player: it
 * sends a MapVote when the player votes, then again every MAP_VOTE_INTERVAL
 * while the waiting room shows another vote for the player, as WaitingRoom
 * does for the ready state, and nothing once the game started.
 */
class MapBallot final : public IMapBallot, public ISessionObserver {
 public:
  /** @param room Shows the votes and whether the game started; it must
   * outlive the ballot. */
  explicit MapBallot(const IWaitingRoom& room);

  void voteFor(std::uint8_t map) override;
  [[nodiscard]] std::optional<std::uint8_t> requestedMap() const override;

  void sessionOpened(const network::LobbyPlayer& ownPlayer,
                     ISessionOutbox& outbox) override;

  /** @brief Forgets the vote asked for. */
  void sessionClosed() override;

  /** @brief Sends the vote again once MAP_VOTE_INTERVAL passed while the
   * waiting room shows another vote for the player. */
  void sessionHeld(engine::Duration elapsed) override;

 private:
  const IWaitingRoom* room_;
  ISessionOutbox* outbox_{nullptr};
  std::uint8_t ownPlayerNumber_{0};
  std::optional<std::uint8_t> requestedMap_;
  engine::Duration sinceVote_{};
};

}  // namespace rtype::client
