#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <string>
#include "Menu.hpp"

/** @returns The point of the playfield at the top-left corner of the bar of a
 * menu whose line reads so, read from what the menu paints. */
[[nodiscard]] sf::Vector2i barCornerOf(const rtype::client::Menu& menu,
                                       const std::string& line);

/** @returns Where the caret lies after a text of so many characters written
 * at an anchor. */
[[nodiscard]] sf::IntRect caretAfter(sf::Vector2i textAnchor, int characters);
