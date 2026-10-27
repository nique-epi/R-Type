#pragma once

#include <SFML/System/Vector2.hpp>
#include "FlightLogLook.hpp"
#include "IInterfaceCanvas.hpp"

namespace rtype::client {

/** @brief Writes the left page of the open book, whose top-left corner is
 * @p page: MISSION LOG, the stamp GAME OVER, ALL PILOTS DOWN, the team score
 * and a polaroid per pilot, each when its time comes. */
void paintMissionLogPage(IInterfaceCanvas& canvas, const FlightLogLook& look,
                         sf::Vector2i page);

}  // namespace rtype::client
