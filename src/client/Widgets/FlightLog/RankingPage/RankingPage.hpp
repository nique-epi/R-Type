#pragma once

#include <SFML/System/Vector2.hpp>
#include "FlightLogLook.hpp"
#include "IInterfaceCanvas.hpp"

namespace rtype::client {

/** @brief Writes the right page of the open book, whose top-left corner is
 * @p page: FINAL RANKING and its rule, then a row per pilot, best first,
 * each written when its time comes. */
void paintRankingPage(IInterfaceCanvas& canvas, const FlightLogLook& look,
                      sf::Vector2i page);

}  // namespace rtype::client
