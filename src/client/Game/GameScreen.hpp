#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "ActionSet.hpp"
#include "BossLook.hpp"
#include "BossPresentation.hpp"
#include "ChatWindow.hpp"
#include "ComponentRegistry.hpp"
#include "DepartureFeed.hpp"
#include "EffectSpawner.hpp"
#include "EnemyActionPlayer.hpp"
#include "EntityFeedback.hpp"
#include "EntityMessage.hpp"
#include "EntityRegistry.hpp"
#include "EntityReplicator.hpp"
#include "GameEnded.hpp"
#include "GameResult.hpp"
#include "GameScreenParts.hpp"
#include "IEntityLayer.hpp"
#include "IEntityMessageSource.hpp"
#include "IInterfaceCanvas.hpp"
#include "IPlayerInputLink.hpp"
#include "IScreen.hpp"
#include "IScreenNavigator.hpp"
#include "IServerTicks.hpp"
#include "IShipLookExchange.hpp"
#include "ISky.hpp"
#include "InterpolationClock.hpp"
#include "Logger.hpp"
#include "MissionBanner.hpp"
#include "OwnPrediction.hpp"
#include "PlayerCommands.hpp"
#include "PlayerPalette.hpp"
#include "ScoreBoard.hpp"
#include "ScreenConstants.hpp"
#include "SilenceWarning.hpp"
#include "SystemScheduler.hpp"
#include "TimeConstants.hpp"
#include "WorldState.hpp"

namespace rtype::client {

struct PlayerSettings;

/**
 * @brief The game in progress: its entities, drawn over the scrolling
 * background, which the frame loop draws under every screen.
 *
 * The screen owns the entities of the game, which appear and disappear with
 * it, the systems that animate them, and the EntityReplicator that applies to
 * them what the server announces and plays the sounds of what it applies,
 * through the sounds of the parts. Each update moves every animation and every
 * hit flash forward by the time elapsed, then hands the replicator the entity
 * messages the server sent since the previous update, whose new effects show
 * their first frame, then repaints the entities on the entity layer; since
 * draw() is const, each draw only lays that layer. The layer is painted once
 * at construction, empty until the server announces an entity. The
 * replicator and the effects point at the registries of the screen, so it
 * never moves.
 *
 * Each update also moves the effects on (see EffectSpawner), then sets the
 * sky. The time of the game, which the warp, the MISSION START banner and the
 * end delay count, and the silence of the server start with the first
 * WorldState or the end of the game, not with the screen: the server sends none
 * while it waits for the other players to load, up to START_BARRIER_SECONDS, so
 * until then the stars keep their own speed, no banner shows and no warning
 * appears. The sky: the speed of its stars from the start of the game (see
 * warpSpeedFactorAt(), 1 when the player reduced flashes), and the shake of the
 * camera, which also shifts the entity layer, never the status band. The sky is
 * put back at its speed and place when the screen goes.
 *
 * The status band of the players lies over the entities: it is painted when
 * the screen is built, from the waiting room the game started from, then
 * again whenever a world state or a PlayerLeft changes a pilot (see
 * ScoreBoard) or the MISSION START banner changes (see missionBannerAt()),
 * never at every frame otherwise. A PlayerLeft marks its pilot as gone: LEFT,
 * or SIGNAL LOST when the server timed it out.
 *
 * The feed of departures lies over the entities with the band: a line for
 * each pilot the band shows gone (see DepartureFeed). From
 * SILENCE_WARNING_DELAY without any message of the server, until the next
 * one, WAITING FOR THE SERVER shows in the top-right corner (see
 * SilenceWarning); never once the game ended. Both are painted with the band,
 * only when they change.
 *
 * Releasing OPTIONS_KEY releases every action held, then puts the options over
 * the game, which goes on running behind them, dimmed. The release, not the
 * press: the system repeats the press of a held key, which would open and
 * close the options again and again.
 *
 * Key and focus events feed the player commands, by the place of the keys on
 * the keyboard and through the keys the player chose: playerActions() tells
 * what the player holds right now, and nothing while the window has no focus,
 * but shoot while AUTO-FIRE is on. The screen assumes the window has the focus
 * when it is built, and learns otherwise from the focus events; only the
 * screen on top receives events, which is why opening the options releases
 * what is held. The actions held are logged each time they change.
 *
 * Each update sends the actions held to the server, the first update included.
 * One input goes out for each simulation tick that passed and the own ship
 * moves at once by the same code as the server (see OwnShipPredictor): the
 * prediction is always on, never a setting of the player. The game goes on
 * behind the options, so it keeps sending them there, with nothing held but
 * shoot while AUTO-FIRE is on, and keeps applying what the server announces.
 *
 * AUTO-FIRE is read in the settings at each call of playerActions(), so a
 * change in the options applies at once. The server keeps its own rate of
 * fire, so the ship fires no faster than with the shoot key held.
 *
 * Each update writes the name of every pilot over its ship once the ships are
 * placed, hidden while PILOT NAMES is off (see tagPilotShips()); the setting
 * is read at each update too.
 *
 * Once the server ends the game, the screen ranks its pilots into the result
 * of the parts, then asks for the end screen in place of every screen
 * END_SCREEN_DELAY later on the clock of the game, once; the game goes on
 * meanwhile, so the last explosion plays.
 *
 * The chat of the session shows over the playfield (ChatWindow), on the status
 * picture, repainted at each update while it shows; while it is open it takes
 * every key, the ship holds nothing, and its ESC opens no options.
 */
class GameScreen : public IScreen {
 public:
  /**
   * @param entityLayer Picture the entities are painted on, never null.
   * @param statusCanvas Picture the status band is painted on, never null.
   * The screen owns both, so they come apart from the parts it borrows.
   */
  GameScreen(GameScreenParts parts, std::unique_ptr<IEntityLayer> entityLayer,
             std::unique_ptr<IInterfaceCanvas> statusCanvas);

  GameScreen(const GameScreen&) = delete;
  GameScreen& operator=(const GameScreen&) = delete;
  GameScreen(GameScreen&&) = delete;
  GameScreen& operator=(GameScreen&&) = delete;
  ~GameScreen() override;

  void handleEvent(const sf::Event& event) override;
  void update(engine::Duration elapsed) override;
  void draw(sf::RenderTarget& target) const override;
  [[nodiscard]] bool hidesScreensBelow() const override;

  /** @returns The actions the player holds right now, and shoot while
   * AUTO-FIRE is on. */
  [[nodiscard]] ActionSet playerActions() const;

 private:
  /** @brief Hands the replicator every entity message the server sent since
   * the previous update, in the order they arrived, and the score board every
   * world state among them. */
  void applyEntityMessages();

  /** @brief Hands @p state, applied by the replicator already, to the score
   * board, the interpolation clock, the inputs awaiting confirmation and the
   * prediction of the own ship. */
  void applyWorldState(const network::WorldState& state);

  /** @brief Repaints the status band when a pilot, the banner or the palette
   * the player chose changed since it was last painted. */
  void repaintStatusWhenChanged();

  /** @brief Paints the status band from the score board, and the banner of
   * the start of the game and @p boss over it, and keeps what was painted. */
  void repaintStatus(const BossLook& boss);

  /** @brief Ranks the pilots of the board by the scores of @p ended into the
   * result of the parts, and remembers when the game ended. */
  void recordEnd(const network::GameEnded& ended);

  /** @brief Asks for the end screen in place of every screen once
   * END_SCREEN_DELAY passed since the game ended, once. */
  void askForTheEndWhenDue();

  IScreenNavigator* navigator_;
  std::unique_ptr<IEntityLayer> entityLayer_;
  std::unique_ptr<IInterfaceCanvas> statusCanvas_;
  const PlayerSettings* settings_;
  engine::EntityRegistry entities_;
  engine::ComponentRegistry components_{entities_};
  EffectSpawner effects_;
  EntityFeedback feedback_;
  OwnPrediction prediction_;
  EntityReplicator replicator_;
  EnemyActionPlayer enemyActions_;
  BossPresentation boss_;
  engine::SystemScheduler systems_;
  PlayerCommands playerCommands_;
  IPlayerInputLink* inputs_;
  IEntityMessageSource* entityMessageSource_;
  const IServerTicks* serverTicks_;
  const IShipLookExchange* shipLooks_;
  std::vector<EntityMessage> receivedEntityMessages_;
  ScoreBoard scoreBoard_;
  GameResult* result_;
  std::uint8_t playerNumber_;
  InterpolationClock interpolationClock_;
  std::optional<engine::Duration> endedAt_;
  std::uint32_t paintedStatusRevision_{};
  logging::Logger logger_{std::string{GAME_SCREEN_LOGGER_NAME}};
  std::uint8_t loggedActions_{0};
  ISky* sky_;
  MissionBannerLook paintedBanner_;
  std::optional<PlayerPalette> paintedPalette_;
  DepartureFeed departures_;
  SilenceWarning silence_;
  ChatWindow chat_;
  std::uint32_t paintedDepartures_{};
  bool paintedSilence_ = false;
  bool paintedChat_ = false;
  BossLook paintedBoss_;
  bool worldSeen_ = false;
};

}  // namespace rtype::client
