#include "GameScreen.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <memory>
#include <utility>
#include <variant>
#include "ActionSet.hpp"
#include "AnimationSystem.hpp"
#include "EntityMessage.hpp"
#include "FlashPreference.hpp"
#include "HitFlashSystem.hpp"
#include "IEntityLayer.hpp"
#include "IEntityMessageSource.hpp"
#include "IInterfaceCanvas.hpp"
#include "IPlayerInputLink.hpp"
#include "IScreenNavigator.hpp"
#include "PlayerInputPacer.hpp"
#include "PlayerSettings.hpp"
#include "ScreenConstants.hpp"
#include "ScreenId.hpp"
#include "StatusBand.hpp"
#include "TimeConstants.hpp"
#include "WorldState.hpp"

namespace rtype::client {

namespace {

/** @returns The flash preference the settings of the player hold. */
FlashPreference flashPreferenceOf(const PlayerSettings& settings) {
  return settings.reduceFlashes ? FlashPreference::Reduced
                                : FlashPreference::Full;
}

}  // namespace

GameScreen::GameScreen(Parts parts, std::unique_ptr<IEntityLayer> entityLayer,
                       std::unique_ptr<IInterfaceCanvas> statusCanvas)
    : navigator_(&parts.navigator.get()),
      entityLayer_(std::move(entityLayer)),
      statusCanvas_(std::move(statusCanvas)),
      settings_(&parts.settings.get()),
      playerCommands_(true, parts.settings.get().keys),
      inputs_(&parts.inputs.get()),
      entityMessageSource_(&parts.entityMessages.get()),
      scoreBoard_(parts.connection.get().lobbyPlayers(),
                  parts.connection.get().status().playerNumber) {
  systems_.add(std::make_unique<AnimationSystem>());
  systems_.add(std::make_unique<HitFlashSystem>());
  entityLayer_->paint(components_);
  repaintStatus();
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
  effects_.setFlashPreference(flashPreferenceOf(*settings_));
  applyEntityMessages();
  entityLayer_->paint(components_);
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
  if (scoreBoard_.revision() != paintedStatusRevision_) {
    repaintStatus();
  }
}

void GameScreen::repaintStatus() {
  statusCanvas_->clear();
  paintStatusBand(*statusCanvas_, scoreBoard_.pilots());
  statusCanvas_->display();
  paintedStatusRevision_ = scoreBoard_.revision();
}

ActionSet GameScreen::playerActions() const { return playerCommands_.held(); }

void GameScreen::draw(sf::RenderTarget& target) const {
  entityLayer_->show(target);
  statusCanvas_->show(target);
}

bool GameScreen::hidesScreensBelow() const { return true; }

}  // namespace rtype::client
