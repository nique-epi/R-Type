#include "MenuBuilderScrollingRowsFixtures.hpp"
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <cstddef>
#include <memory>
#include <vector>
#include "Menu.hpp"
#include "MenuBuilder.hpp"
#include "MenuBuilderScrollingRowsTestConstants.hpp"
#include "PlayfieldMapping.hpp"
#include "PointerEvents.hpp"
#include "RecordingRowControl.hpp"
#include "TextMetrics.hpp"

using rtype::client::Menu;
using rtype::client::MenuBuilder;
using rtype::client::PlayfieldMapping;
using rtype::client::TextMetrics;
using rtype::client::testing::LONG_DESCRIPTION;
using rtype::client::testing::NINE_ROW_LABELS;

Menu panelOfNineRows(RowControlRecord& record) {
  return panelOfNineRowsAt(record, TextMetrics{});
}

Menu panelOfNineRowsAt(RowControlRecord& record, TextMetrics metrics) {
  MenuBuilder builder(MenuBuilder::Frame::TabbedPanel, nullptr, metrics);
  builder.heading("SETTINGS")
      .tabs({"ACCESS"}, 0, {.previous = "Q", .next = "E"}, [](std::size_t) {});
  for (const char* label : NINE_ROW_LABELS) {
    builder.row({.label = label, .description = LONG_DESCRIPTION},
                std::make_unique<RecordingRowControl>(
                    record, std::vector<sf::Keyboard::Key>{}, true));
  }
  return builder.cancelButton({.label = "BACK"}, [] {}).build();
}

sf::Event downPress() {
  return sf::Event::KeyPressed{.code = sf::Keyboard::Key::Down};
}

sf::Event wheelTurn(float delta, sf::Vector2i pixel) {
  return sf::Event::MouseWheelScrolled{.wheel = sf::Mouse::Wheel::Vertical,
                                       .delta = delta,
                                       .position = pixel * INTERFACE_PIXEL};
}

PlayfieldMapping wholePlayfield() { return samePlayfieldPoint; }
