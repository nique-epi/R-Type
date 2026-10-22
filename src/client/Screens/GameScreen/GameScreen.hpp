#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "ActionSet.hpp"
#include "ComponentRegistry.hpp"
#include "EffectSpawner.hpp"
#include "EntityMessage.hpp"
#include "EntityRegistry.hpp"
#include "EntityReplicator.hpp"
#include "FlashPreference.hpp"
#include "IEntityLayer.hpp"
#include "IEntityMessageSource.hpp"
#include "IInterfaceCanvas.hpp"
#include "IPlayerInputLink.hpp"
#include "IScreen.hpp"
#include "IScreenNavigator.hpp"
#include "IServerConnection.hpp"
#include "ISky.hpp"
#include "Logger.hpp"
#include "MissionBanner.hpp"
#include "PlayerCommands.hpp"
#include "PlayerInputPacer.hpp"
#include "ScoreBoard.hpp"
#include "ScreenConstants.hpp"
#include "SystemScheduler.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

struct PlayerSettings;

/**
 * @brief The game in progress: its entities, drawn over the scrolling
 * background, which the frame loop draws under every screen.
 *
 * The screen owns the entities of the game, which appear and disappear with
 * it, the systems that animate them, and the EntityReplicator that applies to
 * them what the server announces. Each update moves every animation and every
 * hit flash forward by the time elapsed, then hands the replicator the entity
 * messages the server sent since the previous update, whose new effects show
 * their first frame, then repaints the entities on the entity layer; since
 * draw() is const, each draw only lays that layer. The layer is painted once
 * at construction, empty until the server announces an entity. The
 * replicator and the effects point at the registries of the screen, so it
 * never moves.
 *
 * Each update also moves the effects on (see EffectSpawner), then sets the
 * sky: the speed of its stars from the start of the game (see
 * warpSpeedFactorAt(), 1 when the player reduced flashes), and the shake of
 * the camera, which also shifts the entity layer, never the status band. The
 * sky is put back at its speed and place when the screen goes.
 *
 * The status band of the players lies over the entities: it is painted when
 * the screen is built, from the waiting room the game started from, then
 * again whenever a world state changes a pilot (see ScoreBoard) or the
 * MISSION START banner changes (see missionBannerAt()), never at every frame
 * otherwise.
 *
 * Releasing OPTIONS_KEY releases every action held, then puts the options over
 * the game, which goes on running behind them, dimmed. The release, not the
 * press: the system repeats the press of a held key, which would open and
 * close the options again and again.
 *
 * Key and focus events feed the player commands, by the place of the keys on
 * the keyboard and through the keys the player chose: playerActions() tells
 * what the player holds right now, and nothing while the window has no focus.
 * The screen assumes the window has the focus when it is built, and learns
 * otherwise from the focus events; only the screen on top receives events,
 * which is why opening the options releases what is held. The actions held
 * are logged each time they change.
 *
 * Each update sends the actions held to the server when the PlayerInputPacer
 * says an input is due, the first update included. The game goes on behind the
 * options, so it keeps sending them there, with nothing held, and keeps
 * applying what the server announces.
 */
class GameScreen : public IScreen {
 public:
  /** @brief What a game screen is built from, but the pictures it owns. */
  struct Parts {
    /** @brief Screens the game lives among, which must outlive it; the
     * options are pushed over the game when OPTIONS_KEY is released. */
    std::reference_wrapper<IScreenNavigator> navigator;
    /** @brief Settings of the player, which must outlive the screen: the keys
     * of the controls, and whether effects reduce their flashes, read at each
     * update. */
    std::reference_wrapper<const PlayerSettings> settings;
    /** @brief Where the actions held go to the server, which must outlive the
     * screen. */
    std::reference_wrapper<IPlayerInputLink> inputs;
    /** @brief Where the entity messages of the server come from, which must
     * outlive the screen. */
    std::reference_wrapper<IEntityMessageSource> entityMessages;
    /** @brief The session the game started in: its waiting room and the
     * number of this client, read when the screen is built. */
    std::reference_wrapper<const IServerConnection> connection;
    /** @brief The sky behind the screens, which must outlive the screen: its
     * stars speed up at the start of the game, and it shakes with the
     * camera. */
    std::reference_wrapper<ISky> sky;
  };

  /**
   * @param entityLayer Picture the entities are painted on, never null.
   * @param statusCanvas Picture the status band is painted on, never null.
   * The screen owns both, so they come apart from the parts it borrows.
   */
  GameScreen(Parts parts, std::unique_ptr<IEntityLayer> entityLayer,
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

  /** @returns The actions the player holds right now. */
  [[nodiscard]] ActionSet playerActions() const;

 private:
  /** @brief Hands the replicator every entity message the server sent since
   * the previous update, in the order they arrived, and the score board every
   * world state among them. */
  void applyEntityMessages();

  /** @brief Repaints the status band when a pilot changed since it was last
   * painted. */
  void repaintStatusWhenChanged();

  /** @brief Paints the status band from the score board, and the banner of
   * the start of the game over it, and keeps what was painted. */
  void repaintStatus();

  /** @brief Sets the speed of the stars and the shake of the sky for the
   * time since the start of the game. */
  void moveSky();

  [[nodiscard]] FlashPreference flashPreference() const;

  IScreenNavigator* navigator_;
  std::unique_ptr<IEntityLayer> entityLayer_;
  std::unique_ptr<IInterfaceCanvas> statusCanvas_;
  const PlayerSettings* settings_;
  engine::EntityRegistry entities_;
  engine::ComponentRegistry components_{entities_};
  EffectSpawner effects_;
  EntityReplicator replicator_{entities_, components_, effects_};
  engine::SystemScheduler systems_;
  PlayerCommands playerCommands_;
  IPlayerInputLink* inputs_;
  IEntityMessageSource* entityMessageSource_;
  std::vector<EntityMessage> receivedEntityMessages_;
  ScoreBoard scoreBoard_;
  std::uint32_t paintedStatusRevision_{};
  PlayerInputPacer inputPacer_;
  logging::Logger logger_{std::string{GAME_SCREEN_LOGGER_NAME}};
  std::uint8_t loggedActions_{0};
  ISky* sky_;
  MissionBannerLook paintedBanner_;
};

}  // namespace rtype::client
