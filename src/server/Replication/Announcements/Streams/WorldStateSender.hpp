#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <vector>
#include "ComponentRegistry.hpp"
#include "DeltaWorldState.hpp"
#include "EntitySpawnSender.hpp"
#include "Logger.hpp"
#include "PlayerRoster.hpp"
#include "WorldState.hpp"

namespace rtype::server {

struct ConnectedPlayer;
class ConnectedPlayers;

/**
 * @brief Sends the state of the world to every connected player of a game,
 * as many times per second as it is built to, spread over the ticks: the state
 * of a tick is sent when the tick is the first of its share of the second.
 *
 * The state lists every connected player with the score the player earned,
 * then every entity that has a NetworkIdentity, a Position and an
 * EntityType, each under its network id. Each player gets it as a
 * DeltaWorldState written against the newest state the player acknowledged
 * (see SentStates), or in full, with no reference, when it acknowledged none
 * within the last MAXIMUM_REFERENCE_DISTANCE ticks. It is sent as
 * several datagrams, with the same tick, when it does not fit one. Each
 * datagram starts with the packet header of its player: its session token and
 * its next sequence (see nextSessionHeader), so each part of a state has a
 * sequence of its own, which SentStates waits to see acknowledged. Nothing is
 * built when no player is connected. A state that needs more than
 * MAXIMUM_DELTA_PARTS datagrams is not sent to the player, and an error says
 * so.
 *
 * An entity whose position is not a finite number, which only a game bug can
 * produce, is left out of the state instead of stopping the server; a warning
 * gives how many entities were left out, each time that number changes.
 */
class WorldStateSender {
 public:
  /**
   * @param roster The players of the game, who receive the state; it must
   * outlive the sender.
   * @param send Called for each datagram to send. Must not be empty.
   * @param statesPerSecond The states sent to each player per second, from
   * LOWEST_WORLD_STATES_PER_SECOND to HIGHEST_WORLD_STATES_PER_SECOND; it need
   * not divide the tick rate.
   * @throws InvalidStateRateException when it is outside that range.
   */
  WorldStateSender(PlayerRoster& roster, SendDatagram send,
                   std::size_t statesPerSecond);

  /**
   * @brief Sends the state to every connected player when @p tick is one that
   * carries a state. Call it once per tick, on the simulation thread.
   *
   * @param tick The tick of the game being simulated, counted by the game from
   * its start; it is the tick the state carries.
   */
  void onTick(std::uint32_t tick, engine::ComponentRegistry& components);

 private:
  /** @returns What the WorldState says of each of @p players: its number, its
   * ship, its lives, its score, whether it can be hit, and its motion: the
   * directions of the Intent of its ship and the maneuver its ship in @p
   * components runs. */
  [[nodiscard]] static std::vector<network::PlayerState> playerStatesOf(
      const ConnectedPlayers& players, engine::ComponentRegistry& components);
  [[nodiscard]] static network::WorldState capture(
      std::uint32_t tick, engine::ComponentRegistry& components,
      std::size_t& leftOutCount);
  void reportLeftOut(std::size_t leftOutCount);
  /** @brief Sends @p player the state as a delta against its acknowledged
   * state, and keeps it, with @p entities, as the newest state sent to it. */
  void sendTo(
      ConnectedPlayer& player, const network::WorldState& state,
      std::shared_ptr<const std::vector<network::EntityState>> entities);
  /** @returns The payloads of @p delta; nothing, after logging an error, when
   * it needs more datagrams than MAXIMUM_DELTA_PARTS. */
  [[nodiscard]] std::optional<std::vector<std::vector<std::byte>>>
  encodedPartsOf(const network::DeltaWorldState& delta);
  /** @returns Whether @p tick is one that carries a state: exactly
   * statesPerSecond_ of every SIMULATION_TICKS_PER_SECOND consecutive ticks
   * are, the first of them included. */
  [[nodiscard]] bool carriesState(std::uint32_t tick) const;

  PlayerRoster* roster_{nullptr};
  SendDatagram send_;
  std::size_t statesPerSecond_;
  std::size_t reportedLeftOutCount_{0};
  logging::Logger logger_;
};

}  // namespace rtype::server
