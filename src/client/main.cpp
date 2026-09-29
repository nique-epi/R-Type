#include <SFML/Graphics.hpp>
#include <optional>

int main() {
  sf::RenderWindow window(sf::VideoMode({800, 600}), "R-Type");
  while (window.isOpen()) {
    while (const std::optional event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>()) {
        window.close();
      }
    }
    window.clear();
    window.display();
  }
  return 0;
}
