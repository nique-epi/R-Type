#pragma once

#include <SFML/System/Vector2.hpp>
#include <optional>
#include "EndChoice.hpp"
#include "FlightLogLook.hpp"
#include "IInterfaceCanvas.hpp"

namespace rtype::client {

/** @brief Paints, once the call starts, PRESS ENTER TO REPLAY typed then
 * blinking, and the QUIT button, each in its state. */
void paintEndChoices(IInterfaceCanvas& canvas, const FlightLogLook& look);

/** @returns The choice under a pixel of the interface picture: PROMPT_AREA
 * replays, QUIT_BUTTON quits; none elsewhere. */
[[nodiscard]] std::optional<EndChoice> choiceAt(sf::Vector2i pixel);

}  // namespace rtype::client
