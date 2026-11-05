#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include "ComponentRegistry.hpp"
#include "ConnectedPlayer.hpp"
#include "EntitySheet.hpp"
#include "EntitySpawnSender.hpp"
#include "GameContext.hpp"
#include "GameEndAnnouncer.hpp"
#include "GameOutcome.hpp"
#include "IGameReplication.hpp"
#include "LobbyStateAnnouncer.hpp"
#include "PlayerRoster.hpp"
#include "ShipSpawnAnnouncer.hpp"
#include "World.hpp"
#include "WorldStateSender.hpp"

namespace rtype::server {

/**
 * @brief Tells the players how the game stands: the room while it waits, the
 * ships that appear, the world while it is in progress and the result once it
 * ended by itself. It owns the announcers and the sender that do it.
 *
 * The announcers that work on one world are built again for each new one (see
 * onWorldBuilt()); the world sender lives as long as the game. onWorldBuilt()
 * must be called before any other function, since the ship and the room are
 * announced for a world.
 *
 * Not thread-safe: use it from the simulation thread only.
 */
class GameReplication final : public IGameReplication {
 public:
  /**
   * @param game The players the announcements go to, how to send, the world
   * states each player is sent per second (LOWEST_WORLD_STATES_PER_SECOND to
   * HIGHEST_WORLD_STATES_PER_SECOND) and the sheets, whose SHIP_SHEET_NAME is
   * the ship of every player; it must outlive this.
   * @throws InvalidStateRateException when the rate is outside that range.
   * @throws game::UnknownEntityTypeException when no sheet is named
   * SHIP_SHEET_NAME.
   */
  explicit GameReplication(GameContext& game);

  GameReplication(const GameReplication&) = delete;
  GameReplication& operator=(const GameReplication&) = delete;
  GameReplication(GameReplication&&) = delete;
  GameReplication& operator=(GameReplication&&) = delete;
  ~GameReplication() override = default;

  void onWorldBuilt(game::World& world) override;
  void onPlayerJoined(ConnectedPlayer& player) override;
  void onWaitingTick() override;
  void onGameFinished(GameOutcome outcome) override;
  void onFinishedTick() override;
  void onProgressTick(std::uint32_t tick,
                      engine::ComponentRegistry& components) override;

 private:
  PlayerRoster& roster_;
  SendDatagram send_;
  std::reference_wrapper<const game::EntitySheet> ship_;
  WorldStateSender worldStateSender_;
  std::unique_ptr<ShipSpawnAnnouncer> shipSpawnAnnouncer_;
  std::unique_ptr<LobbyStateAnnouncer> lobbyStateAnnouncer_;
  std::unique_ptr<GameEndAnnouncer> gameEndAnnouncer_;
};

}  // namespace rtype::server
