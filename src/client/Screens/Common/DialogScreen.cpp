#include "DialogScreen.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <memory>
#include <utility>
#include "IInterfaceCanvas.hpp"
#include "Menu.hpp"
#include "PlayfieldMapping.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

DialogScreen::DialogScreen(Menu menu, PlayfieldMapping playfieldPointAt,
                           std::unique_ptr<IInterfaceCanvas> canvas)
    : canvas_(std::move(canvas)),
      playfieldPointAt_(std::move(playfieldPointAt)),
      menu_(std::move(menu)) {
  repaint();
}

void DialogScreen::handleEvent(const sf::Event& event) {
  menu_.handleEvent(event, playfieldPointAt_);
}

void DialogScreen::update(engine::Duration elapsed) {
  menu_.advance(elapsed);
  repaint();
}

void DialogScreen::draw(sf::RenderTarget& target) const {
  canvas_->show(target);
}

bool DialogScreen::hidesScreensBelow() const { return false; }

void DialogScreen::repaint() {
  canvas_->clear();
  menu_.paint(*canvas_);
  canvas_->display();
}

}  // namespace rtype::client
