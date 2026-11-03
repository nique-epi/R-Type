#include "InkTag.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <string>
#include "IInterfaceCanvas.hpp"
#include "InkTagConstants.hpp"
#include "TextStyle.hpp"

namespace rtype::client {

int inkTagWidth(const IInterfaceCanvas& canvas, const std::string& word) {
  return canvas.textWidth(word, TextFont::Small) + INK_TAG_PADDING;
}

void paintInkTag(IInterfaceCanvas& canvas, const InkTag& tag) {
  canvas.fill(
      sf::IntRect(tag.topLeft, {inkTagWidth(canvas, tag.word), INK_TAG_HEIGHT}),
      INK_TAG_COLOR);
  canvas.write(tag.word, tag.topLeft + INK_TAG_TEXT_OFFSET,
               {.font = TextFont::Small, .color = tag.wordColor});
}

}  // namespace rtype::client
