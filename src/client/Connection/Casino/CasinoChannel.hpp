#pragma once

#include <cstdint>
#include <optional>
#include <vector>
#include "CasinoPlace.hpp"
#include "ChipMoves.hpp"
#include "ICasinoChannel.hpp"
#include "ISessionObserver.hpp"
#include "ISessionOutbox.hpp"
#include "IWaitingRoom.hpp"
#include "LobbyState.hpp"
#include "MessageDispatcher.hpp"
#include "RouletteBets.hpp"
#include "RouletteRound.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The roulette table of the waiting room, seen from the client: it
 * sends the place and the bets of the player and keeps the rounds the server
 * sends.
 *
 * chooseTable() sends a CasinoPlace at once, then every
 * CASINO_MESSAGE_INTERVAL until the server shows it: a round listing the
 * player for the table, a LobbyState that does not show it at the casino for
 * the room. placeBets() sends a RouletteBets at once, then every interval
 * while the bets of that round stay open; a new round drops the bets of the
 * one before, as the server does.
 *
 * A round older than the newest one kept (RFC 1982) is dropped. The first
 * round seen in Spinning or Result for a number settles it: its stake leaves
 * the chips, its payout comes in, once, however many copies follow; the
 * server knows the payout from the draw on. A player whose session closes
 * while the wheel turns keeps what the server settled without it. The rounds
 * start over when the game starts: the table closes then.
 *
 * Not thread-safe: use it from the thread that updates the session.
 */
class CasinoChannel final : public ICasinoChannel, public ISessionObserver {
 public:
  /**
   * @param dispatcher Where the handler of RouletteRound is registered; it
   * must outlive the channel.
   * @param room Tells the places of the room and whether the game started; it
   * must outlive the channel.
   */
  CasinoChannel(network::MessageDispatcher& dispatcher,
                const IWaitingRoom& room);

  void chooseTable(network::TablePlace place) override;
  void placeBets(std::vector<network::RouletteBet> bets,
                 bool spinVote) override;
  [[nodiscard]] std::optional<network::RouletteRound> round() const override;
  [[nodiscard]] ChipMoves takeChipMoves() override;

  void sessionOpened(const network::LobbyPlayer& ownPlayer,
                     ISessionOutbox& outbox) override;
  void sessionClosed() override;
  void sessionHeld(engine::Duration elapsed) override;

 private:
  /** @brief Keeps @p received when it is not older, and settles its round. */
  void onRound(const network::RouletteRound& received);

  /** @brief Sends what the player asked the server has not shown yet. */
  void sendWhatIsPending();

  /** @brief Forgets the rounds, as the table closes. */
  void forgetTable();

  const IWaitingRoom* room_;
  ISessionOutbox* outbox_{nullptr};
  std::uint8_t ownPlayerNumber_{0};
  std::optional<network::TablePlace> requestedPlace_;
  std::optional<network::RouletteRound> round_;
  std::optional<network::RouletteBets> bets_;
  std::optional<std::uint16_t> settledRound_;
  ChipMoves moves_;
  engine::Duration sinceSent_{};
};

}  // namespace rtype::client
