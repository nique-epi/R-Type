#include "GameFlow.hpp"
#include <algorithm>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include "BossStage.hpp"
#include "ConnectedPlayer.hpp"
#include "ConnectedPlayers.hpp"
#include "EntityIdConstants.hpp"
#include "EventBus.hpp"
#include "FavorThePlayers.hpp"
#include "GameContext.hpp"
#include "GameInstanceConstants.hpp"
#include "GameOutcome.hpp"
#include "GameState.hpp"
#include "GameStateChanged.hpp"
#include "IGameReplication.hpp"
#include "Logger.hpp"
#include "MatchSystems.hpp"
#include "MatchSystemsParts.hpp"
#include "PlayerLives.hpp"
#include "PlayerRoster.hpp"
#include "World.hpp"
#include "WorldSlot.hpp"

namespace rtype::server {

namespace {

/**
 * @returns Whether @p player has a life left, the one of a ship it waits for
 * included.
 */
bool hasALifeLeft(game::World& world, const ConnectedPlayer* player) {
  return livesOf(world, *player) > 0;
}

/** @brief Why a game finished, and how it ended. */
struct GameEnding {
  std::string_view reason;
  GameOutcome outcome{GameOutcome::Defeat};
};

/**
 * @returns Why a game played by @p roster in @p world is lost: no player is
 * left in it, or no player left has a life. Nothing otherwise.
 */
std::optional<std::string_view> reasonToLose(PlayerRoster& roster,
                                             game::World& world) {
  const ConnectedPlayers players = roster.connected();
  if (players.empty()) {
    return NO_PLAYER_LEFT_FINISH_REASON;
  }
  if (std::ranges::none_of(players, [&world](const ConnectedPlayer* player) {
        return hasALifeLeft(world, player);
      })) {
    return NO_LIFE_LEFT_FINISH_REASON;
  }
  return std::nullopt;
}

/**
 * @returns How the game in progress played by @p roster in @p world, whose
 * level ends as @p stage says, ends now: lost when no player is left or none
 * of them has a life, otherwise won once the level is cleared: its last wave
 * over, and its boss, if it has one, gone with every other enemy. The loss
 * comes first, so a game lost on the tick its level is cleared is lost.
 * Nothing when it goes on.
 */
std::optional<GameEnding> endingOf(PlayerRoster& roster, game::World& world,
                                   const BossStage& stage) {
  const std::optional<std::string_view> loss = reasonToLose(roster, world);
  if (loss.has_value()) {
    return GameEnding{.reason = *loss, .outcome = GameOutcome::Defeat};
  }
  if (stage.levelCleared()) {
    return GameEnding{.reason = stage.hasBoss() ? BOSS_DEFEATED_FINISH_REASON
                                                : LAST_WAVE_OVER_FINISH_REASON,
                      .outcome = GameOutcome::Victory};
  }
  return std::nullopt;
}

/** @brief Moves @p state to @p next and tells @p events so, with the state it
 * left. */
void changeState(GameState& state, GameState next, engine::EventBus& events) {
  events.publish(GameStateChanged{.previous = std::exchange(state, next),
                                  .current = next});
}

/** @brief Writes that the game started, with how many players. */
void logStart(const logging::Logger& logger, PlayerRoster& roster) {
  logger.info("game started, players connected: ", roster.connected().size());
}

}  // namespace

GameFlow::GameFlow(GameContext& game, WorldSlot& world,
                   IGameReplication& replication)
    : game_(game),
      worldSlot_(world),
      replication_(replication),
      startBarrier_(game.roster.get(), game.send),
      hitArbitration_(std::make_unique<FavorThePlayers>()),
      logger_(std::string{INSTANCES_LOGGER_NAME}) {}

void GameFlow::begin() { startNewWorld(); }

GameState GameFlow::state() const { return state_; }

std::uint32_t GameFlow::tick() const { return tick_; }

game::World& GameFlow::world() { return worldSlot_.current(); }

void GameFlow::onTick() {
  finishIfOver();
  advanceState();
  if (state_ == GameState::InProgress) {
    replication_.onProgressTick(tick_, world().components());
  }
}

void GameFlow::advanceState() {
  switch (state_) {
    case GameState::WaitingForPlayers:
      if (!game_.everyPlayerIsReady()) {
        replication_.onWaitingTick();
        break;
      }
      changeState(state_, GameState::Starting, game_.events.get());
      startBarrier_.open();
      break;
    case GameState::Starting:
      if (startBarrier_.advance()) {
        changeState(state_, GameState::InProgress, game_.events.get());
        logStart(logger_, game_.roster.get());
      }
      break;
    case GameState::InProgress:
      ++tick_;
      systems_->onTick(*hitArbitration_, tick_);
      break;
    case GameState::Finished:
      if (++finishedTicks_ < FINISHED_STATE_TICKS) {
        replication_.onFinishedTick();
        break;
      }
      startNewWorld();
      changeState(state_, GameState::WaitingForPlayers, game_.events.get());
      break;
  }
}

void GameFlow::finish() {
  if (state_ == GameState::InProgress) {
    changeState(state_, GameState::Finished, game_.events.get());
    finishedTicks_ = 0;
  }
}

void GameFlow::finishIfOver() {
  if (state_ != GameState::InProgress) {
    return;
  }
  const std::optional<GameEnding> ending =
      endingOf(game_.roster.get(), world(), systems_->bossStage());
  if (!ending.has_value()) {
    return;
  }
  logger_.info("game finished: ", ending->reason);
  finish();
  replication_.onGameFinished(ending->outcome);
}

void GameFlow::buildWorld() {
  game::World& world = worldSlot_.renew();
  replication_.onWorldBuilt(world);
  systems_ = std::make_unique<MatchSystems>(
      MatchSystemsParts{.game = game_,
                        .world = world,
                        .waveSeed = game_.settings.seed + worldsBuilt_,
                        .respawn = [this](ConnectedPlayer& player) {
                          replication_.onPlayerJoined(player);
                        }});
}

void GameFlow::startNewWorld() {
  buildWorld();
  ++worldsBuilt_;
  tick_ = 0;
  finishedTicks_ = 0;
  for (ConnectedPlayer* const player : game_.roster.get().connected()) {
    player->ready = false;
    player->score = 0;
    player->livesWithoutShip = game_.settings.rules.lives;
    player->ticksUntilRespawn = 0;
    player->shipEntityId = identifiers::NO_ENTITY_ID;
    replication_.onPlayerJoined(*player);
  }
}

}  // namespace rtype::server
