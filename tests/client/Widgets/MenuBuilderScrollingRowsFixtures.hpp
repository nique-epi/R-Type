#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include "Menu.hpp"
#include "RecordingRowControl.hpp"

/** @returns A tabbed panel of nine rows, ROW 1 to ROW 9, and BACK. */
[[nodiscard]] rtype::client::Menu panelOfNineRows(RowControlRecord& record);

/** @returns A press of the Down key. */
[[nodiscard]] sf::Event downPress();

/** @returns A turn of the wheel by @p delta notches over @p pixel of the
 * interface, given in window pixels as the window reports it. */
[[nodiscard]] sf::Event wheelTurn(float delta, sf::Vector2i pixel);

/** @returns The mapping of a window as large as the playfield. */
[[nodiscard]] rtype::client::PlayfieldMapping wholePlayfield();
