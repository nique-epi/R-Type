#include "Label.hpp"
#include <SFML/System/Vector2.hpp>
#include <string>
#include <utility>
#include "IInterfaceCanvas.hpp"
#include "TextStyle.hpp"

namespace rtype::client {

Label::Label(std::string text, sf::Vector2i anchor, TextStyle style)
    : text_(std::move(text)), anchor_(anchor), style_(style) {}

void Label::paint(IInterfaceCanvas& canvas) const {
  canvas.write(text_, anchor_, style_);
}

}  // namespace rtype::client
