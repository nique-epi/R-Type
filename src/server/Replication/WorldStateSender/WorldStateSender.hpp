#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <span>
#include <vector>
#include "ComponentRegistry.hpp"
#include "ConnectedPlayer.hpp"
#include "Logger.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"
#include "WorldState.hpp"

namespace rtype::server {

/**
 * @brief Sends the state of the world to every connected player of a game,
 * on the ticks that are a multiple of TICKS_PER_WORLD_STATE.
 *
 * The state lists every connected player with the score the player earned,
 * then every entity that has a NetworkIdentity, a Position and an
 * EntityType, each under its network id. It is sent as several datagrams, with
 * the same tick, when the entities do not fit one. Each datagram starts with
 * the packet header of its player: its session token and its next sequence
 * (see nextSessionHeader), so each part of a state has a sequence of its own.
 * Nothing is built when no player is connected.
 *
 * An entity whose position is not a finite number, which only a game bug can
 * produce, is left out of the state instead of stopping the server; a warning
 * gives how many entities were left out, each time that number changes.
 */
class WorldStateSender {
 public:
  /**
   * @brief What the sender calls for each datagram to send.
   */
  using Send = std::function<void(network::OutgoingDatagram)>;

  /**
   * @param roster The players of the game, who receive the state; it must
   * outlive the sender.
   * @param send Must not be empty.
   */
  WorldStateSender(PlayerRoster& roster, Send send);

  /**
   * @brief Sends the state to every connected player when @p tick is one that
   * carries a state. Call it once per tick, on the simulation thread.
   *
   * @param tick The tick of the game being simulated, counted by the game from
   * its start; it is the tick the state carries.
   */
  void onTick(std::uint32_t tick, engine::ComponentRegistry& components);

 private:
  [[nodiscard]] static std::vector<network::PlayerState> playerStatesOf(
      std::span<ConnectedPlayer* const> players);
  [[nodiscard]] static network::WorldState capture(
      std::uint32_t tick, engine::ComponentRegistry& components,
      std::size_t& leftOutCount);
  void reportLeftOut(std::size_t leftOutCount);

  PlayerRoster* roster_{nullptr};
  Send send_;
  std::size_t reportedLeftOutCount_{0};
  logging::Logger logger_;
};

}  // namespace rtype::server
