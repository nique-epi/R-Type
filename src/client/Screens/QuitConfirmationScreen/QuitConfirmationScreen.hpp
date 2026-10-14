#pragma once

#include <memory>
#include "IInterfaceCanvas.hpp"
#include "IScreen.hpp"
#include "IScreenNavigator.hpp"
#include "Menu.hpp"
#include "PlayfieldMapping.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief Asks whether to quit, in a dialog over the dimmed screens below.
 *
 * NO is selected at first, and MENU_CANCEL_KEY answers it too: both close the
 * dialog. YES closes every screen, which closes the client. Only the screen
 * on top receives events, so the arrows and Tab stay in the dialog while it is
 * open.
 */
class QuitConfirmationScreen : public IScreen {
 public:
  /**
   * @param navigator Screens the dialog lives among, which must outlive it.
   * @param canvas Picture the dialog is painted on, never null.
   * @param playfieldPointAt Gives the point of the playfield under a pixel of
   * the window.
   */
  QuitConfirmationScreen(IScreenNavigator& navigator,
                         std::unique_ptr<IInterfaceCanvas> canvas,
                         PlayfieldMapping playfieldPointAt);

  void handleEvent(const sf::Event& event) override;
  void update(engine::Duration elapsed) override;
  void draw(sf::RenderTarget& target) const override;

 private:
  [[nodiscard]] static Menu buildMenu(IScreenNavigator& navigator);
  void repaint();

  std::unique_ptr<IInterfaceCanvas> canvas_;
  PlayfieldMapping playfieldPointAt_;
  Menu menu_;
};

}  // namespace rtype::client
