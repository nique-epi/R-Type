#include "GameScreen.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <memory>
#include <random>
#include <utility>
#include <variant>
#include "ActionSet.hpp"
#include "AnimationSystem.hpp"
#include "EffectSpawner.hpp"
#include "EntityFeedback.hpp"
#include "EntityMessage.hpp"
#include "FlashPreference.hpp"
#include "GameScreenParts.hpp"
#include "GroundEffectSystem.hpp"
#include "HitFlashSystem.hpp"
#include "IEntityLayer.hpp"
#include "IEntityMessageSource.hpp"
#include "IInterfaceCanvas.hpp"
#include "IPlayerInputLink.hpp"
#include "IScreenNavigator.hpp"
#include "ISky.hpp"
#include "MissionBanner.hpp"
#include "MissionStart.hpp"
#include "PlayerInputPacer.hpp"
#include "PlayerPreferences.hpp"
// NOLINTNEXTLINE(misc-include-cleaner)
#include "PlayerSettings.hpp"
#include "ScreenConstants.hpp"
#include "ScreenId.hpp"
#include "SpriteEffectSystem.hpp"
#include "StatusBand.hpp"
#include "TimeConstants.hpp"
#include "WorldState.hpp"

namespace rtype::client {

namespace {

/** @brief Sets the speed of the stars and the shake of the sky for the time
 * since the start of the game: the warp of MISSION START with flashes, their
 * own speed without. */
void moveSky(ISky& sky, const EffectSpawner& effects,
             FlashPreference preference) {
  sky.setSpeedFactor(preference == FlashPreference::Full
                         ? warpSpeedFactorAt(effects.sinceStart())
                         : 1.0F);
  sky.setOffset(effects.cameraOffset());
}

}  // namespace

GameScreen::GameScreen(GameScreenParts parts,
                       std::unique_ptr<IEntityLayer> entityLayer,
                       std::unique_ptr<IInterfaceCanvas> statusCanvas)
    : navigator_(&parts.navigator.get()),
      entityLayer_(std::move(entityLayer)),
      statusCanvas_(std::move(statusCanvas)),
      settings_(&parts.settings.get()),
      effects_(entities_, components_, std::random_device{}()),
      replicator_(entities_, components_,
                  EntityFeedback{.effects = effects_, .sounds = parts.sounds}),
      playerCommands_(true, parts.settings.get().keys),
      inputs_(&parts.inputs.get()),
      entityMessageSource_(&parts.entityMessages.get()),
      scoreBoard_(parts.connection.get().lobbyPlayers(),
                  parts.connection.get().status().playerNumber),
      sky_(&parts.sky.get()) {
  systems_.add(std::make_unique<AnimationSystem>());
  systems_.add(std::make_unique<HitFlashSystem>());
  systems_.add(std::make_unique<SpriteEffectSystem>());
  systems_.add(std::make_unique<GroundEffectSystem>());
  moveSky(*sky_, effects_, flashPreferenceOf(*settings_));
  entityLayer_->paint(components_, effects_.cameraOffset());
  repaintStatus();
}

GameScreen::~GameScreen() {
  sky_->setSpeedFactor(1.0F);
  sky_->setOffset({});
}

void GameScreen::handleEvent(const sf::Event& event) {
  const auto* released = event.getIf<sf::Event::KeyReleased>();
  if (released != nullptr && released->code == OPTIONS_KEY) {
    playerCommands_.releaseAll();
    navigator_->push(ScreenId::GameOptions);
  }
  if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
    playerCommands_.keyPressed(keyPressed->scancode);
  } else if (released != nullptr) {
    playerCommands_.keyReleased(released->scancode);
  } else if (event.is<sf::Event::FocusLost>()) {
    playerCommands_.focusLost();
  } else if (event.is<sf::Event::FocusGained>()) {
    playerCommands_.focusGained();
  }
}

void GameScreen::update(engine::Duration elapsed) {
  const ActionSet held = playerCommands_.held();
  if (inputPacer_.advance(elapsed)) {
    inputs_->sendPlayerInput(held);
  }
  if (held.bits() != loggedActions_) {
    logger_.trace("actions held: ", static_cast<unsigned int>(held.bits()));
    loggedActions_ = held.bits();
  }
  systems_.run(components_, elapsed);
  const FlashPreference preference = flashPreferenceOf(*settings_);
  effects_.advance(elapsed, preference);
  applyEntityMessages();
  moveSky(*sky_, effects_, preference);
  entityLayer_->paint(components_, effects_.cameraOffset());
  repaintStatusWhenChanged();
}

void GameScreen::applyEntityMessages() {
  entityMessageSource_->drainEntityMessages(receivedEntityMessages_);
  for (const EntityMessage& message : receivedEntityMessages_) {
    std::visit([this](const auto& announced) { replicator_.apply(announced); },
               message);
    if (const auto* state = std::get_if<network::WorldState>(&message)) {
      scoreBoard_.apply(*state);
    }
  }
  receivedEntityMessages_.clear();
}

void GameScreen::repaintStatusWhenChanged() {
  if (scoreBoard_.revision() != paintedStatusRevision_ ||
      missionBannerAt(effects_.sinceStart()) != paintedBanner_) {
    repaintStatus();
  }
}

void GameScreen::repaintStatus() {
  const MissionBannerLook banner = missionBannerAt(effects_.sinceStart());
  statusCanvas_->clear();
  paintStatusBand(*statusCanvas_, scoreBoard_.pilots());
  paintMissionBanner(*statusCanvas_, banner);
  statusCanvas_->display();
  paintedStatusRevision_ = scoreBoard_.revision();
  paintedBanner_ = banner;
}

ActionSet GameScreen::playerActions() const { return playerCommands_.held(); }

void GameScreen::draw(sf::RenderTarget& target) const {
  entityLayer_->show(target);
  statusCanvas_->show(target);
}

bool GameScreen::hidesScreensBelow() const { return true; }

}  // namespace rtype::client
