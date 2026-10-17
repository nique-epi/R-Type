#include "LinkText.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <functional>
#include <string>
#include <utility>
#include "BarState.hpp"
#include "IInterfaceCanvas.hpp"
#include "RowControlConstants.hpp"
#include "TextStyle.hpp"
#include "WidgetConstants.hpp"
#include "WidgetText.hpp"

namespace rtype::client {

LinkText::LinkText(std::string text, std::function<void()> action)
    : text_(std::move(text) + LINK_SUFFIX), action_(std::move(action)) {}

int LinkText::width() const { return regularTextWidth(text_); }

void LinkText::paint(IInterfaceCanvas& canvas, sf::IntRect area,
                     BarState row) const {
  canvas.write(text_, {area.position.x, area.position.y + CONTROL_TEXT_TOP},
               {.font = TextFont::Regular, .color = barTextColor(row)});
}

bool LinkText::isActivatable() const { return true; }

void LinkText::activate() {
  if (action_ != nullptr) {
    action_();
  }
}

}  // namespace rtype::client
