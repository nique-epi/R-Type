#pragma once

#include <memory>
#include "IInterfaceCanvas.hpp"
#include "IScreen.hpp"
#include "IScreenNavigator.hpp"
#include "ISoundPlayer.hpp"
#include "Menu.hpp"
#include "PlayfieldMapping.hpp"
#include "Squadron.hpp"
#include "TimeConstants.hpp"
#include "TitleScreenParts.hpp"

namespace rtype::client {

struct PlayerSettings;

/**
 * @brief The screen the client opens on: the logo, PLAY, SETTINGS and QUIT
 * over the scrolling background, and the squadron flying in on the right.
 *
 * PLAY puts the connection screen over the title, which it hides; SETTINGS
 * puts the options over the title, which hide it; QUIT, or MENU_CANCEL_KEY,
 * puts the quit confirmation over it, which dims it. The title keeps its
 * selection under those screens, and its squadron goes on moving. The menu and
 * the squadron are painted at each update and laid at each draw; they are
 * painted once when the screen is built, so its first frame already shows them.
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
  [[nodiscard]] static Menu buildMenu(IScreenNavigator& navigator,
                                      ISoundPlayer& sounds);
  void repaint();

  std::unique_ptr<IInterfaceCanvas> canvas_;
  PlayfieldMapping playfieldPointAt_;
  const PlayerSettings* settings_;
  Menu menu_;
  Squadron squadron_;
};

}  // namespace rtype::client
