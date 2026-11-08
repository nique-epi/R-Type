#include "MenuBuilderPanelEndBarsFixtures.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <cstddef>
#include <memory>
#include <vector>
#include "Menu.hpp"
#include "MenuBuilder.hpp"
#include "RecordingRowControl.hpp"

using rtype::client::Menu;
using rtype::client::MenuBuilder;

Menu panelWithQuitAfterBack(RowControlRecord& record) {
  return MenuBuilder(MenuBuilder::Frame::TabbedPanel)
      .heading("SETTINGS")
      .tabs({"SOUND", "CONTROLS"}, 0, {.previous = "Q", .next = "E"},
            [](std::size_t) {})
      .row({.label = "WINDOW SIZE", .description = "> SIZES"},
           std::make_unique<RecordingRowControl>(
               record, std::vector<sf::Keyboard::Key>{}, true))
      .cancelButton({.label = "BACK"}, [] {})
      .button({.label = "QUIT"}, [] {})
      .build();
}
