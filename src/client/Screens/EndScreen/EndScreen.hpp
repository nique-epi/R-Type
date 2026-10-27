#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "EndChoice.hpp"
#include "EndScreenParts.hpp"
#include "FinalPilot.hpp"
#include "FlashPreference.hpp"
#include "FlightLogLook.hpp"
#include "FlightLogSchedule.hpp"
#include "IInterfaceCanvas.hpp"
#include "IScreen.hpp"
#include "IScreenNavigator.hpp"
#include "IServerConnection.hpp"
#include "ISoundPlayer.hpp"
#include "MotionPreference.hpp"
#include "PlayfieldMapping.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The end of a game: the flight log of the squadron, from the result
 * the game wrote, then PRESS ENTER TO REPLAY and QUIT.
 *
 * The book drops, a taunt is typed on its label, the cover turns, the left
 * page tells the defeat and the right page ranks the pilots, then the call to
 * play again blinks under the book and QUIT waits under its corner (see
 * paintFlightLog()). The taunt is drawn from the seed of the parts among
 * those that fit the pilots, never the previous one.
 *
 * Until the choices show, MENU_CONFIRM_KEY, MENU_CANCEL_KEY or a click skip
 * to the last picture; no other key does, so a key of the game still held
 * skips nothing. Then MENU_CONFIRM_KEY presses PLAY AGAIN and MENU_CANCEL_KEY
 * QUIT, from anywhere; the pointer lights the choice under it and a click
 * presses it. A key held down acts once. A pressed choice shows for
 * PRESS_DURATION, then the book closes and leaves for EXIT_TIME, then PLAY
 * AGAIN asks for the waiting room after a game, and QUIT leaves the server
 * and asks for the title, each in place of every screen; nothing is heard
 * afterwards. The screen stays whatever the server sends meanwhile.
 *
 * With reduced motion, the screen opens on its last picture and leaves at
 * once after the press.
 */
class EndScreen : public IScreen {
 public:
  /**
   * @param canvas Picture the flight log is painted on, never null: the
   * screen owns it, so it comes apart from the parts it borrows.
   */
  EndScreen(EndScreenParts parts, std::unique_ptr<IInterfaceCanvas> canvas);

  EndScreen(const EndScreen&) = delete;
  EndScreen& operator=(const EndScreen&) = delete;
  EndScreen(EndScreen&&) = delete;
  EndScreen& operator=(EndScreen&&) = delete;
  ~EndScreen() override = default;

  void handleEvent(const sf::Event& event) override;
  void update(engine::Duration elapsed) override;
  void draw(sf::RenderTarget& target) const override;
  [[nodiscard]] bool hidesScreensBelow() const override;

 private:
  /** @brief Hovers the choice under the pointer, or skips or presses on a
   * left click. */
  void handlePointer(const sf::Event& event);

  /** @brief Skips to the last picture while the choices do not show, or
   * presses @p choice, if any, once they do; nothing once a choice is
   * pressed. */
  void respond(std::optional<EndChoice> choice);

  /** @brief Holds the press, then lets the book leave, then leaves. */
  void advanceExit(engine::Duration elapsed);

  /** @brief Asks for the screen of @p choice in place of every screen,
   * leaving the server first for QUIT. */
  void leave(EndChoice choice);

  /** @returns The flight log as it looks now. */
  [[nodiscard]] FlightLogLook lookNow() const;

  void repaint();

  IScreenNavigator* navigator_;
  IServerConnection* connection_;
  ISoundPlayer* sounds_;
  std::unique_ptr<IInterfaceCanvas> canvas_;
  PlayfieldMapping playfieldPointAt_;
  MotionPreference motion_{};
  FlashPreference flashes_{};
  std::vector<FinalPilot> pilots_;
  std::vector<std::string> taunt_;
  FlightLogSchedule schedule_;
  engine::Duration sinceStart_{};
  std::optional<sf::Keyboard::Key> heldKey_;
  std::optional<EndChoice> hovered_;
  std::optional<EndChoice> pressed_;
  engine::Duration pressedFor_{};
};

}  // namespace rtype::client
