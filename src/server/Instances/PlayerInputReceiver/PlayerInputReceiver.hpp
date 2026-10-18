#pragma once

#include <cstddef>
#include <span>
#include "Endpoint.hpp"
#include "IncomingDatagram.hpp"
#include "Logger.hpp"
#include "MessageDispatcher.hpp"
#include "MessageOrigin.hpp"
#include "PlayerInput.hpp"
#include "PlayerRoster.hpp"

namespace rtype::server {

/**
 * @brief Reads the inputs among the datagrams of the players, and keeps in
 * ConnectedPlayer::newestInput the PlayerInput with the highest inputNumber
 * each player sent, as the Inputs section of the protocol page describes. An
 * input that is not newer than the one kept, because it arrived late or
 * twice, changes nothing. inputNumber is compared as a plain number: a session
 * sending 60 inputs per second would take more than 800 days to wrap it.
 *
 * A datagram is read only when its header carries this protocol and version
 * with no flag set, its messages add up to its end, its sender is in the
 * roster, and it carries the session token of that player. A datagram with
 * another token is dropped and counted: whoever forges the address of a
 * player could otherwise send an input numbered 4294967295, above every input
 * the player will ever send, and hold the ship of that player on the forged
 * keys for the rest of the session. The sequence is not checked yet.
 *
 * Every message of a datagram read is handled in order: a PlayerInput is kept
 * when it is newer, a PlayerInput its reader refuses is dropped, and any other
 * message is skipped. Nothing caused by the content of a datagram makes it
 * throw. It is neither copied nor moved: the handler of its dispatcher points
 * at it.
 */
class PlayerInputReceiver {
 public:
  /** @param roster The players of the game; it must outlive the receiver. */
  explicit PlayerInputReceiver(PlayerRoster& roster);

  PlayerInputReceiver(const PlayerInputReceiver&) = delete;
  PlayerInputReceiver& operator=(const PlayerInputReceiver&) = delete;
  PlayerInputReceiver(PlayerInputReceiver&&) = delete;
  PlayerInputReceiver& operator=(PlayerInputReceiver&&) = delete;
  ~PlayerInputReceiver() = default;

  /** @brief Reads @p datagrams, oldest first. */
  void receive(std::span<const network::IncomingDatagram> datagrams);

  /**
   * @returns How many datagrams were dropped because they carry another
   * session token than the one of the player they come from.
   */
  [[nodiscard]] std::size_t foreignTokenCount() const;

 private:
  void read(const network::IncomingDatagram& datagram);
  void dropForeignToken(const network::Endpoint& sender);
  void keepIfNewer(const network::MessageOrigin& origin,
                   const network::PlayerInput& input);

  PlayerRoster* roster_;
  network::MessageDispatcher dispatcher_;
  std::size_t foreignTokenCount_{0};
  logging::Logger logger_;
};

}  // namespace rtype::server
