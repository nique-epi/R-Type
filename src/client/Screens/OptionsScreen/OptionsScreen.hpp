#pragma once

#include <memory>
#include "IInterfaceCanvas.hpp"
#include "IScreen.hpp"
#include "IScreenNavigator.hpp"
#include "Menu.hpp"
#include "PlayfieldMapping.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

class GameWindow;

/**
 * @brief Options pushed over the title or the game, which they hide: the sky,
 * a bar per size of WINDOW_SIZES, then BACK. The game goes on running behind
 * them.
 *
 * A size too large for the desktop is a disabled bar; the size of the window
 * has a filled check box, the others an empty one. The size lives in
 * GameWindow, which outlives the screen: the bars are set from it at each
 * update. BACK, or MENU_CANCEL_KEY, closes the options and gives the events
 * back to the screen below.
 *
 * Provisional: it stands in for the options menu until that one exists.
 */
class OptionsScreen : public IScreen {
 public:
  /**
   * @param navigator Screens the options live among, which must outlive them.
   * @param window Window whose size the options select, which must outlive
   * the screen.
   * @param canvas Picture the options are painted on, never null.
   * @param playfieldPointAt Gives the point of the playfield under a pixel of
   * the window.
   */
  OptionsScreen(IScreenNavigator& navigator, GameWindow& window,
                std::unique_ptr<IInterfaceCanvas> canvas,
                PlayfieldMapping playfieldPointAt);

  void handleEvent(const sf::Event& event) override;
  void update(engine::Duration elapsed) override;
  void draw(sf::RenderTarget& target) const override;
  [[nodiscard]] bool hidesScreensBelow() const override;

 private:
  [[nodiscard]] static Menu buildMenu(IScreenNavigator& navigator,
                                      GameWindow& window);

  /** @brief Disables the bars of the sizes too large for the desktop, and
   * checks the size of the window. */
  void showWindowSizes();

  void repaint();

  GameWindow* window_;
  std::unique_ptr<IInterfaceCanvas> canvas_;
  PlayfieldMapping playfieldPointAt_;
  Menu menu_;
};

}  // namespace rtype::client
