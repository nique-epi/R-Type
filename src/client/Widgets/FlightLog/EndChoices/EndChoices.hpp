#pragma once

#include <SFML/System/Vector2.hpp>
#include <optional>
#include "EndChoice.hpp"
#include "EndChoiceAreas.hpp"
#include "FlightLogLook.hpp"
#include "IInterfaceCanvas.hpp"
#include "TextMetrics.hpp"

namespace rtype::client {

/** @brief Paints, once the call starts, PRESS ENTER TO REPLAY typed then
 * blinking, and the QUIT button, each in its state. */
void paintEndChoices(IInterfaceCanvas& canvas, const FlightLogLook& look);

/** @returns Where the choices lie at the text size of @p metrics: QUIT_BUTTON
 * and the call on PROMPT_TOP, centered, as drawn at the size the fonts are
 * drawn at; above it, QUIT as wide and tall as its label needs, and the call
 * centered as wide as its letters, under QUIT when QUIT reaches its row. */
[[nodiscard]] EndChoiceAreas endChoiceAreasOf(TextMetrics metrics);

/** @returns The choice under a pixel of the interface picture, at the text
 * size of @p metrics: the area of the call replays, QUIT quits; none
 * elsewhere. */
[[nodiscard]] std::optional<EndChoice> choiceAt(sf::Vector2i pixel,
                                                TextMetrics metrics);

}  // namespace rtype::client
