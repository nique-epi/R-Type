#include "LostScreen.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>
#include <memory>
#include <optional>
#include <utility>
#include "BarState.hpp"
#include "ConnectionPhase.hpp"
#include "FlashPreference.hpp"
#include "GlowingCallPainting.hpp"
#include "HeldKey.hpp"
#include "IInterfaceCanvas.hpp"
#include "InterfaceConstants.hpp"
#include "InterfacePixel.hpp"
#include "LostScreenLook.hpp"
#include "LostScreenParts.hpp"
#include "MotionPreference.hpp"
#include "NoSignalCard.hpp"
#include "NoSignalConstants.hpp"
#include "NoSignalLook.hpp"
#include "NoSignalReason.hpp"
#include "PlayfieldConstants.hpp"
#include "ScreenId.hpp"
#include "ScreenOff.hpp"
#include "SoundConstants.hpp"
#include "TimeConstants.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

namespace {

/** @returns What the card says of how the session ended. */
NoSignalReason reasonOf(ConnectionPhase ending) {
  return ending == ConnectionPhase::Closed ? NoSignalReason::ClosedServer
                                           : NoSignalReason::SilentServer;
}

/** @returns When the whole call shows: the screen turned off, the delay of
 * the call passed, and every letter typed. */
engine::Duration callReadyAt() {
  return SCREEN_OFF_TIME + RECONNECT_CALL_DELAY +
         callTypingTimeOf(RECONNECT_CALL_TEXT);
}

/** @returns The state of the call: pressed, under the pointer, or at rest. */
BarState callStateOf(bool pressed, bool hovered) {
  if (pressed) {
    return BarState::Pressed;
  }
  return hovered ? BarState::Selected : BarState::Idle;
}

/** @brief Draws @p picture across the playfield, squeezed into @p rows rows
 * of the interface picture around its middle row. */
void drawSqueezed(sf::RenderTarget& target, const sf::Texture& picture,
                  int rows) {
  const float share =
      static_cast<float>(rows) / static_cast<float>(INTERFACE_AREA.size.y);
  sf::Sprite sprite(picture);
  sprite.setScale(
      {game::PLAYFIELD_WIDTH / static_cast<float>(picture.getSize().x),
       share * game::PLAYFIELD_HEIGHT /
           static_cast<float>(picture.getSize().y)});
  sprite.setPosition({0.0F, game::PLAYFIELD_HEIGHT * (1.0F - share) /
                                static_cast<float>(SIDES_PER_AXIS)});
  target.draw(sprite);
}

/** @returns What the picture shows @p sinceStart after the client gave up,
 * with @p card for the card: the screen turning off, then the card. */
LostScreenLook lookAt(engine::Duration sinceStart, const NoSignalLook& card) {
  if (sinceStart < SCREEN_OFF_TIME) {
    return {.card = false,
            .screenOff = screenOffAt(sinceStart),
            .call = BarState::Idle,
            .callLetters = 0};
  }
  return {.card = true,
          .screenOff = {},
          .call = card.call,
          .callLetters = lettersShownOf(reconnectCallOf(card))};
}

}  // namespace

LostScreen::LostScreen(LostScreenParts parts,
                       std::unique_ptr<IInterfaceCanvas> canvas)
    : navigator_(&parts.navigator.get()),
      sounds_(&parts.sounds.get()),
      canvas_(std::move(canvas)),
      picture_(std::move(parts.picture)),
      reason_(reasonOf(parts.ending)),
      playfieldPointAt_(std::move(parts.playfieldPointAt)),
      steady_(parts.motion == MotionPreference::Reduced ||
              parts.flashes == FlashPreference::Reduced) {
  if (parts.motion == MotionPreference::Reduced) {
    sinceStart_ = callReadyAt();
  }
  repaint();
}

void LostScreen::handleEvent(const sf::Event& event) {
  if (const auto* pressed = event.getIf<sf::Event::KeyPressed>()) {
    const bool repeated =
        std::exchange(heldKey_, pressed->code) == pressed->code;
    if (!repeated && pressed->code == MENU_CONFIRM_KEY) {
      reconnect();
    } else if (!repeated && pressed->code == MENU_CANCEL_KEY) {
      quit();
    }
  } else if (releasesHeldKey(event, heldKey_)) {
    heldKey_.reset();
  } else {
    handlePointer(event);
  }
}

void LostScreen::handlePointer(const sf::Event& event) {
  if (const auto* moved = event.getIf<sf::Event::MouseMoved>()) {
    hovered_ = RECONNECT_CALL_AREA.contains(
        interfacePixelAt(playfieldPointAt_(moved->position)));
  } else if (const auto* clicked = event.getIf<sf::Event::MouseButtonPressed>();
             clicked != nullptr && clicked->button == sf::Mouse::Button::Left &&
             (sinceStart_ < SCREEN_OFF_TIME ||
              RECONNECT_CALL_AREA.contains(
                  interfacePixelAt(playfieldPointAt_(clicked->position))))) {
    reconnect();
  }
}

void LostScreen::reconnect() {
  if (leaving_) {
    return;
  }
  if (sinceStart_ < SCREEN_OFF_TIME) {
    sinceStart_ = SCREEN_OFF_TIME;
    repaint();
    return;
  }
  leaving_ = true;
  pressedFor_ = engine::Duration::zero();
  sounds_->play(CLICK_SOUND_ID);
}

void LostScreen::quit() {
  if (leaving_) {
    return;
  }
  leaving_ = true;
  navigator_->replaceAll(ScreenId::Title);
}

void LostScreen::update(engine::Duration elapsed) {
  sinceStart_ += elapsed;
  advanceExit(elapsed);
  repaint();
}

void LostScreen::advanceExit(engine::Duration elapsed) {
  if (!pressedFor_.has_value() || *pressedFor_ >= PRESS_DURATION) {
    return;
  }
  *pressedFor_ += elapsed;
  if (*pressedFor_ >= PRESS_DURATION) {
    navigator_->replaceAll(ScreenId::Title);
    navigator_->push(ScreenId::Connection);
  }
}

void LostScreen::repaint() {
  const NoSignalLook card{
      .reason = reason_,
      .sinceCard = sinceStart_ - SCREEN_OFF_TIME,
      .call = callStateOf(pressedFor_.has_value(), hovered_),
      .steady = steady_};
  const LostScreenLook look = lookAt(sinceStart_, card);
  if (painted_ == look) {
    return;
  }
  canvas_->clear();
  if (look.card) {
    paintNoSignalCard(*canvas_, card);
  } else {
    paintScreenOff(*canvas_, look.screenOff);
  }
  canvas_->display();
  painted_ = look;
}

void LostScreen::draw(sf::RenderTarget& target) const {
  canvas_->show(target);
  if (picture_.has_value() && painted_.has_value() &&
      painted_->screenOff.pictureRows > 0) {
    drawSqueezed(target, *picture_, painted_->screenOff.pictureRows);
  }
}

bool LostScreen::hidesScreensBelow() const { return true; }

}  // namespace rtype::client
