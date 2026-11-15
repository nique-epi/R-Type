#pragma once

#include <memory>
#include "IInterfaceCanvas.hpp"
#include "IScreen.hpp"
#include "IScreenNavigator.hpp"
#include "ISoundPlayer.hpp"
#include "IUpdateNotice.hpp"
#include "Menu.hpp"
#include "PlayStart.hpp"
#include "PlayfieldMapping.hpp"
#include "Squadron.hpp"
#include "TextSize.hpp"
#include "TimeConstants.hpp"
#include "TitleScreenParts.hpp"

namespace rtype::client {

struct PlayerSettings;

/**
 * @brief The screen the client opens on: the logo, PLAY, SETTINGS and QUIT
 * over the scrolling background, and the squadron flying in on the right.
 *
 * PLAY puts the server map over the title, which it hides, or, for a client
 * launched with `-h` (PlayStart::GivenServer), the connection screen that
 * joins at once the server of the last attempt, that of `-h` before any;
 * SETTINGS puts the options over the title, which hide it; QUIT, or
 * MENU_CANCEL_KEY, puts the quit confirmation over it, which dims it. The title
 * keeps its selection under those screens, and its squadron goes on moving. The
 * menu and the squadron are painted at each update and laid at each draw; they
 * are painted once when the screen is built, so its first frame already shows
 * them. Once the update module says a release waits, it puts the update
 * panel over itself; once it says notes wait, WHAT'S NEW; each once.
 */
class TitleScreen : public IScreen {
 public:
  /** @param canvas Picture the menu and the squadron are painted on, never
   * null. */
  TitleScreen(TitleScreenParts parts, std::unique_ptr<IInterfaceCanvas> canvas);

  void handleEvent(const sf::Event& event) override;
  void update(engine::Duration elapsed) override;
  void draw(sf::RenderTarget& target) const override;
  [[nodiscard]] bool hidesScreensBelow() const override;

 private:
  /** @returns The menu of the title, laid out at the text size of the
   * settings. */
  [[nodiscard]] Menu buildMenu() const;

  /** @brief Lays the menu out again when the text size changed since it was
   * built, as the options change it over the title. */
  void followTextSize();
  void repaint();
  /** @brief Puts the update panel over the title once a release waits, or
   * else WHAT'S NEW once notes wait; each once per title. */
  void askForNotice();

  std::unique_ptr<IInterfaceCanvas> canvas_;
  PlayfieldMapping playfieldPointAt_;
  const PlayerSettings* settings_;
  IScreenNavigator* navigator_;
  ISoundPlayer* sounds_;
  PlayStart playStart_;
  TextSize builtSize_;
  const IUpdateNotice* notice_;
  bool updateAsked_{};
  bool notesAsked_{};
  Menu menu_;
  Squadron squadron_;
};

}  // namespace rtype::client
