#include "MenuBuilderDialogRowsFixtures.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include <vector>
#include "Menu.hpp"
#include "MenuBuilder.hpp"
#include "MenuPainting.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingRowControl.hpp"

using rtype::client::Menu;
using rtype::client::MenuBuilder;

Menu dialogWithoutRows() {
  return MenuBuilder(MenuBuilder::Frame::Dialog)
      .heading("CONFIRM")
      .message("ARE YOU SURE?")
      .cancelButton({.label = "NO"}, [] {})
      .button({.label = "YES"}, [] {})
      .build();
}

InterfaceCanvasRecord threeRowDialogPainting() {
  RowControlRecord first;
  RowControlRecord second;
  RowControlRecord third;
  const std::vector<sf::Keyboard::Key> used{sf::Keyboard::Key::Right};
  return paintingOf(
      MenuBuilder(MenuBuilder::Frame::Dialog)
          .heading("FILTER")
          .row({.label = "FIRST"},
               std::make_unique<RecordingRowControl>(first, used, false))
          .row({.label = "SECOND"},
               std::make_unique<RecordingRowControl>(second, used, false))
          .row({.label = "THIRD"},
               std::make_unique<RecordingRowControl>(third, used, false))
          .cancelButton({.label = "BACK"}, [] {})
          .button({.label = "APPLY"}, [] {})
          .build());
}

Menu dialogWithRows(RowControlRecord& first, RowControlRecord& second) {
  const std::vector<sf::Keyboard::Key> used{sf::Keyboard::Key::Right};
  return MenuBuilder(MenuBuilder::Frame::Dialog)
      .heading("FILTER")
      .row({.label = "FIRST"},
           std::make_unique<RecordingRowControl>(first, used, false))
      .row({.label = "SECOND"},
           std::make_unique<RecordingRowControl>(second, used, false))
      .cancelButton({.label = "BACK"}, [] {})
      .button({.label = "APPLY"}, [] {})
      .build();
}
