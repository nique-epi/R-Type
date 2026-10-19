#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include "ActionSet.hpp"
#include "ComponentRegistry.hpp"
#include "EffectSpawner.hpp"
#include "EntityRegistry.hpp"
#include "IEntityLayer.hpp"
#include "IPlayerInputLink.hpp"
#include "IScreen.hpp"
#include "IScreenNavigator.hpp"
#include "Logger.hpp"
#include "PlayerCommands.hpp"
#include "PlayerInputPacer.hpp"
#include "ScreenConstants.hpp"
#include "SpriteShowcase.hpp"
#include "SystemScheduler.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

struct PlayerSettings;

/**
 * @brief The game in progress: its entities, drawn over the scrolling
 * background, which the frame loop draws under every screen.
 *
 * The screen owns the entities of the game, which appear and disappear with
 * it, and the systems that animate them. Each update moves every animation and
 * every hit flash forward by the time elapsed, then plays the showcase, whose
 * new effects show their first frame, then repaints the entities on the entity
 * layer; since draw() is const, each draw only lays that layer. The layer is
 * painted once at construction, so the first frame already shows them. Until
 * the client creates the entities the server announces, the screen holds the
 * provisional sprite showcase, which plays every effect in a loop.
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
 * options, so it keeps sending them there, with nothing held.
 */
class GameScreen : public IScreen {
 public:
  /**
   * @param navigator Screens the game lives among, which must outlive it;
   * the options are pushed over the game when OPTIONS_KEY is released.
   * @param entityLayer Picture the entities are painted on, never null.
   * @param settings Settings of the player, which must outlive the screen:
   * the keys of the controls, and whether effects reduce their flashes, read
   * at each update.
   * @param inputs Where the actions held go to the server, which must outlive
   * the screen.
   */
  GameScreen(IScreenNavigator& navigator,
             std::unique_ptr<IEntityLayer> entityLayer,
             const PlayerSettings& settings, IPlayerInputLink& inputs);

  void handleEvent(const sf::Event& event) override;
  void update(engine::Duration elapsed) override;
  void draw(sf::RenderTarget& target) const override;
  [[nodiscard]] bool hidesScreensBelow() const override;

  /** @returns The actions the player holds right now. */
  [[nodiscard]] ActionSet playerActions() const;

 private:
  IScreenNavigator* navigator_;
  std::unique_ptr<IEntityLayer> entityLayer_;
  const PlayerSettings* settings_;
  engine::EntityRegistry entities_;
  engine::ComponentRegistry components_{entities_};
  EffectSpawner effects_{entities_, components_};
  SpriteShowcase showcase_{entities_, components_, effects_};
  engine::SystemScheduler systems_;
  PlayerCommands playerCommands_;
  IPlayerInputLink* inputs_;
  PlayerInputPacer inputPacer_;
  logging::Logger logger_{std::string{GAME_SCREEN_LOGGER_NAME}};
  std::uint8_t loggedActions_{0};
};

}  // namespace rtype::client
