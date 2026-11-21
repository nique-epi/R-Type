#include "WaitingCodePainting.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <string>
#include "IInterfaceCanvas.hpp"
#include "SignInLines.hpp"
#include "SignInScreenConstants.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

void paintWaitingCode(IInterfaceCanvas& canvas, sf::IntRect area,
                      const SignInLines& lines) {
  const sf::Vector2i middleOfTop(
      area.position.x + (area.size.x / SIDES_PER_AXIS), area.position.y);
  canvas.write(lines.title, middleOfTop, SIGN_IN_CODE_STYLE);
  canvas.write(lines.line, area.position + SIGN_IN_PAGE_LINE_OFFSET,
               SIGN_IN_PAGE_LINE_STYLE);
  canvas.write(lines.validity.value_or(std::string{}),
               area.position + SIGN_IN_VALIDITY_OFFSET, SIGN_IN_VALIDITY_STYLE);
}

}  // namespace rtype::client
