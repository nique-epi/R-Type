#include "QuitConfirmationScreen.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <memory>
#include <utility>
#include "IInterfaceCanvas.hpp"
#include "IScreenNavigator.hpp"
#include "Menu.hpp"
#include "MenuBuilder.hpp"
#include "MenuFrame.hpp"
#include "PlayfieldMapping.hpp"
#include "ScreenConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

QuitConfirmationScreen::QuitConfirmationScreen(
    IScreenNavigator& navigator, std::unique_ptr<IInterfaceCanvas> canvas,
    PlayfieldMapping playfieldPointAt)
    : canvas_(std::move(canvas)),
      playfieldPointAt_(std::move(playfieldPointAt)),
      menu_(buildMenu(navigator)) {
  repaint();
}

void QuitConfirmationScreen::handleEvent(const sf::Event& event) {
  menu_.handleEvent(event, playfieldPointAt_);
}

void QuitConfirmationScreen::update(engine::Duration elapsed) {
  menu_.advance(elapsed);
  repaint();
}

void QuitConfirmationScreen::draw(sf::RenderTarget& target) const {
  canvas_->show(target);
}

Menu QuitConfirmationScreen::buildMenu(IScreenNavigator& navigator) {
  return MenuBuilder(MenuFrame::Dialog)
      .heading(CONFIRM_HEADING)
      .message(QUIT_QUESTION)
      .cancelButton({.label = NO_LABEL}, [&navigator] { navigator.pop(); })
      .button({.label = YES_LABEL}, [&navigator] { navigator.closeAll(); })
      .build();
}

void QuitConfirmationScreen::repaint() {
  canvas_->clear();
  menu_.paint(*canvas_);
  canvas_->display();
}

}  // namespace rtype::client
