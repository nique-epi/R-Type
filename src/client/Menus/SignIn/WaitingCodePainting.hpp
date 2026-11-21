#pragma once

#include <SFML/Graphics/Rect.hpp>
#include "IInterfaceCanvas.hpp"
#include "SignInLines.hpp"

namespace rtype::client {

/**
 * @brief Paints the code of @p lines while it waits in @p area, the area the
 * sign-in dialog reserves for it: the code large, in gold, over the middle of
 * the area, then the page to enter it at and how long it stays valid against
 * its left side, as SIGN_IN_CODE_STYLE and the offsets next to it say.
 */
void paintWaitingCode(IInterfaceCanvas& canvas, sf::IntRect area,
                      const SignInLines& lines);

}  // namespace rtype::client
