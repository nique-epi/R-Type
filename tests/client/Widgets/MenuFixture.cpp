#include "MenuFixture.hpp"
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <string>
#include <vector>
#include "KeyPress.hpp"
#include "Menu.hpp"
#include "MenuBuilder.hpp"
#include "MenuFixtures.hpp"
#include "PointerEvents.hpp"
#include "RecordingSoundPlayer.hpp"

using rtype::client::Menu;
using rtype::client::MenuBuilder;

MenuFixture::MenuFixture()
    : menu_(
          MenuBuilder(MenuBuilder::Frame::Page, &sounds_)
              .button({.label = "A"}, [this] { activated_.emplace_back("A"); })
              .button({.label = "B"}, [this] { activated_.emplace_back("B"); })
              .cancelButton({.label = "C"},
                            [this] { activated_.emplace_back("C"); })
              .build()) {}

Menu& MenuFixture::menu() { return menu_; }

const std::vector<std::string>& MenuFixture::activated() const {
  return activated_;
}

void MenuFixture::send(const sf::Event& event) {
  menu_.handleEvent(event, samePlayfieldPoint);
}

void MenuFixture::tap(sf::Keyboard::Key key) {
  send(keyPress(key));
  send(keyRelease(key));
}

sf::Vector2i MenuFixture::barCorner(const std::string& label) {
  return barCornerOf(menu_, label);
}

const RecordingSoundPlayer& MenuFixture::sounds() const { return sounds_; }
