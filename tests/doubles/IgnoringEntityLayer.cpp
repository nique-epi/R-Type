#include "IgnoringEntityLayer.hpp"
#include <SFML/System/Vector2.hpp>

using rtype::engine::ComponentRegistry;

void IgnoringEntityLayer::paint([[maybe_unused]] ComponentRegistry& components,
                                [[maybe_unused]] sf::Vector2i offset) {}

void IgnoringEntityLayer::show(
    [[maybe_unused]] sf::RenderTarget& target) const {}
