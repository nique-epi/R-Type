#include "MenuWithAFieldFixture.hpp"
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <string>
#include <vector>
#include "Menu.hpp"
#include "MenuBuilder.hpp"
#include "MenuPainting.hpp"
#include "MenuTestConstants.hpp"
#include "PointerEvents.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "TextField.hpp"

using rtype::client::Menu;
using rtype::client::MenuBuilder;
using rtype::client::TextField;
using rtype::client::testing::FOUR_BYTES;
using rtype::client::testing::INSIDE_A_BAR;

MenuWithAFieldFixture::MenuWithAFieldFixture()
    : menu_(MenuBuilder(MenuBuilder::Frame::Form)
                .textField({.label = "NAME", .description = "> TYPE A NAME"},
                           TextField("AB", FOUR_BYTES,
                                     TextField::Characters::Printable),
                           [this] { activated_.emplace_back("NAME"); })
                .button({.label = "OK"},
                        [this] { activated_.emplace_back("OK"); })
                .build()) {}

Menu& MenuWithAFieldFixture::menu() { return menu_; }

const std::vector<std::string>& MenuWithAFieldFixture::activated() const {
  return activated_;
}

void MenuWithAFieldFixture::send(const sf::Event& event) {
  menu_.handleEvent(event, samePlayfieldPoint);
}

InterfaceCanvasRecord MenuWithAFieldFixture::painting() const {
  return paintingOf(menu_);
}

sf::Vector2i MenuWithAFieldFixture::pointOn(const std::string& text) const {
  const InterfaceCanvasRecord record = painting();
  const RecordedWriting* writing = writingOf(record, text);
  return writing == nullptr
             ? sf::Vector2i()
             : (writing->anchor * INTERFACE_PIXEL) + INSIDE_A_BAR;
}
