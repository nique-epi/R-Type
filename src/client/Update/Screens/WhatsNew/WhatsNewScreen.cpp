#include "WhatsNewScreen.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <cstddef>
#include <memory>
#include <string>
#include <utility>
#include "CutCornerBox.hpp"
#include "HeldKey.hpp"
#include "IInterfaceCanvas.hpp"
#include "InterfaceColorConstants.hpp"
#include "InterfaceConstants.hpp"
#include "TimeConstants.hpp"
#include "WhatsNewConstants.hpp"
#include "WhatsNewScreenParts.hpp"
#include "WidgetColors.hpp"
#include "WidgetConstants.hpp"
#include "WidgetStyles.hpp"

namespace rtype::client {

WhatsNewScreen::WhatsNewScreen(WhatsNewScreenParts parts,
                               std::unique_ptr<IInterfaceCanvas> canvas)
    : canvas_(std::move(canvas)),
      navigator_(&parts.navigator.get()),
      notes_(std::move(parts.notes)),
      onClose_(std::move(parts.onClose)) {
  paint();
}

void WhatsNewScreen::handleEvent(const sf::Event& event) {
  if (const auto* pressed = event.getIf<sf::Event::KeyPressed>()) {
    const bool repeated =
        std::exchange(heldKey_, pressed->code) == pressed->code;
    if (!repeated && (pressed->code == MENU_CONFIRM_KEY ||
                      pressed->code == MENU_CANCEL_KEY)) {
      close();
    }
  } else if (releasesHeldKey(event, heldKey_)) {
    heldKey_.reset();
  } else if (event.is<sf::Event::MouseButtonPressed>()) {
    close();
  }
}

void WhatsNewScreen::update([[maybe_unused]] engine::Duration elapsed) {}

void WhatsNewScreen::draw(sf::RenderTarget& target) const {
  canvas_->show(target);
}

bool WhatsNewScreen::hidesScreensBelow() const { return false; }

void WhatsNewScreen::close() {
  if (std::exchange(closed_, true)) {
    return;
  }
  onClose_();
  navigator_->pop();
}

void WhatsNewScreen::paint() {
  const sf::IntRect panel{
      (INTERFACE_AREA.size - WHATS_NEW_PANEL_SIZE) / SIDES_PER_AXIS,
      WHATS_NEW_PANEL_SIZE};
  canvas_->clear();
  canvas_->fill(INTERFACE_AREA, DIM_LAYER_COLOR);
  CutCornerBox(panel,
               {.border = PANEL_BORDER_COLOR, .inside = PANEL_INSIDE_COLOR})
      .paint(*canvas_);
  canvas_->write(std::string{WHATS_NEW_HEADING} + notes_.versionText,
                 panel.position + DIALOG_HEADING_OFFSET, DIALOG_HEADING_STYLE);
  canvas_->fill({panel.position + WHATS_NEW_RULE_OFFSET, WHATS_NEW_RULE_SIZE},
                RULE_COLOR);
  for (std::size_t line = 0; line < notes_.lines.size(); ++line) {
    const sf::Vector2i down{0, static_cast<int>(line) * WHATS_NEW_LINE_PITCH};
    canvas_->write(NOTE_BULLET, panel.position + WHATS_NEW_BULLET_OFFSET + down,
                   NOTE_BULLET_STYLE);
    canvas_->write(notes_.lines[line],
                   panel.position + WHATS_NEW_LINE_OFFSET + down,
                   NOTE_LINE_STYLE);
  }
  canvas_->write(WHATS_NEW_HINT, panel.position + WHATS_NEW_HINT_OFFSET,
                 WHATS_NEW_HINT_STYLE);
  canvas_->display();
}

}  // namespace rtype::client
