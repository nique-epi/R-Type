#include "EndScreen.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <cstddef>
#include <memory>
#include <optional>
#include <random>
#include <string_view>
#include <utility>
#include "BarState.hpp"
#include "BookExit.hpp"
#include "EndChoice.hpp"
#include "EndChoices.hpp"
#include "EndScreenParts.hpp"
#include "FlightLogLook.hpp"
#include "FlightLogPainting.hpp"
#include "FlightLogTimeline.hpp"
#include "FlightLogTimingConstants.hpp"
#include "IInterfaceCanvas.hpp"
#include "InterfacePixel.hpp"
#include "LobbyScreenIdConstants.hpp"
#include "MenuScreenIdConstants.hpp"
#include "MotionPreference.hpp"
#include "ScreenId.hpp"
#include "SoundConstants.hpp"
#include "TauntConstants.hpp"
#include "Taunts.hpp"
#include "TimeConstants.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The state a choice is drawn in: pressed, under the pointer, or
 * at rest. */
BarState stateOf(EndChoice choice, std::optional<EndChoice> pressed,
                 std::optional<EndChoice> hovered) {
  if (pressed == choice) {
    return BarState::Pressed;
  }
  return hovered == choice ? BarState::Selected : BarState::Idle;
}

/** @returns The choice a key presses: PLAY AGAIN for MENU_CONFIRM_KEY, QUIT
 * for MENU_CANCEL_KEY, none for any other key. */
std::optional<EndChoice> choiceOfKey(sf::Keyboard::Key key) {
  if (key == MENU_CONFIRM_KEY) {
    return EndChoice::Replay;
  }
  if (key == MENU_CANCEL_KEY) {
    return EndChoice::Quit;
  }
  return std::nullopt;
}

/** @returns Whether @p event lets go of the key held: its release, or the
 * window losing the focus, which hides that release. */
bool releasesHeldKey(const sf::Event& event,
                     std::optional<sf::Keyboard::Key> heldKey) {
  const auto* released = event.getIf<sf::Event::KeyReleased>();
  return event.is<sf::Event::FocusLost>() ||
         (released != nullptr && heldKey == released->code);
}

}  // namespace

EndScreen::EndScreen(EndScreenParts parts,
                     std::unique_ptr<IInterfaceCanvas> canvas)
    : navigator_(&parts.navigator.get()),
      connection_(&parts.connection.get()),
      sounds_(&parts.sounds.get()),
      canvas_(std::move(canvas)),
      playfieldPointAt_(std::move(parts.playfieldPointAt)),
      motion_(parts.motion),
      flashes_(parts.flashes),
      palette_(parts.palette),
      pilots_(parts.result.get().pilots) {
  std::mt19937 random(parts.seed);
  std::optional<std::size_t>& previousTaunt = parts.previousTaunt.get();
  const std::size_t taunt = pickTaunt(pilots_, random, previousTaunt);
  previousTaunt = taunt;
  taunt_ = tauntLinesOf(TAUNTS.at(taunt), pilots_);
  schedule_ = flightLogScheduleOf(pilots_, taunt_);
  if (motion_ == MotionPreference::Reduced) {
    sinceStart_ = schedule_.ready;
  }
  repaint();
}

void EndScreen::handleEvent(const sf::Event& event) {
  if (const auto* pressed = event.getIf<sf::Event::KeyPressed>()) {
    const bool repeated =
        std::exchange(heldKey_, pressed->code) == pressed->code;
    const std::optional<EndChoice> choice = choiceOfKey(pressed->code);
    if (!repeated && choice.has_value()) {
      respond(choice);
    }
  } else if (releasesHeldKey(event, heldKey_)) {
    heldKey_.reset();
  } else {
    handlePointer(event);
  }
}

void EndScreen::handlePointer(const sf::Event& event) {
  if (const auto* moved = event.getIf<sf::Event::MouseMoved>()) {
    hovered_ = choiceAt(interfacePixelAt(playfieldPointAt_(moved->position)));
  } else if (const auto* clicked = event.getIf<sf::Event::MouseButtonPressed>();
             clicked != nullptr && clicked->button == sf::Mouse::Button::Left) {
    respond(choiceAt(interfacePixelAt(playfieldPointAt_(clicked->position))));
  }
}

void EndScreen::respond(std::optional<EndChoice> choice) {
  if (pressed_.has_value()) {
    return;
  }
  if (sinceStart_ < schedule_.ready) {
    sinceStart_ = schedule_.ready;
    return;
  }
  if (choice.has_value()) {
    pressed_ = choice;
    sounds_->play(CLICK_SOUND_ID);
  }
}

void EndScreen::update(engine::Duration elapsed) {
  const engine::Duration before = sinceStart_;
  sinceStart_ += elapsed;
  for (const std::string_view sound :
       cuesBetween(schedule_, before, sinceStart_)) {
    sounds_->play(sound);
  }
  advanceExit(elapsed);
  repaint();
}

void EndScreen::advanceExit(engine::Duration elapsed) {
  if (!pressed_.has_value()) {
    return;
  }
  const engine::Duration leaving =
      PRESS_DURATION + (motion_ == MotionPreference::Reduced
                            ? engine::Duration::zero()
                            : EXIT_TIME);
  const bool stillShown = pressedFor_ < leaving;
  pressedFor_ += elapsed;
  if (stillShown && pressedFor_ >= leaving) {
    leave(*pressed_);
  }
}

void EndScreen::leave(EndChoice choice) {
  if (choice == EndChoice::Quit) {
    connection_->cancel();
    navigator_->replaceAll(TITLE_SCREEN);
    return;
  }
  navigator_->replaceAll(LOBBY_AFTER_GAME_SCREEN);
}

FlightLogLook EndScreen::lookNow() const {
  BookExit exit;
  if (pressed_.has_value() && pressedFor_ >= PRESS_DURATION) {
    exit = BookExit{.choice = pressed_, .age = pressedFor_ - PRESS_DURATION};
  }
  return FlightLogLook{.sinceStart = sinceStart_,
                       .schedule = schedule_,
                       .pilots = pilots_,
                       .taunt = taunt_,
                       .exit = exit,
                       .replay = stateOf(EndChoice::Replay, pressed_, hovered_),
                       .quit = stateOf(EndChoice::Quit, pressed_, hovered_),
                       .motion = motion_,
                       .flashes = flashes_,
                       .palette = palette_};
}

void EndScreen::repaint() {
  canvas_->clear();
  paintFlightLog(*canvas_, lookNow());
  canvas_->display();
}

void EndScreen::draw(sf::RenderTarget& target) const { canvas_->show(target); }

bool EndScreen::hidesScreensBelow() const { return true; }

}  // namespace rtype::client
