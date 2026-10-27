#pragma once

#include <SFML/System/Vector2.hpp>
#include <cstddef>

/** @returns The middle of a line of the open list of sizes, by its order. */
sf::Vector2i lineMiddle(std::size_t line);
