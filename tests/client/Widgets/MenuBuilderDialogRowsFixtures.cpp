#include "MenuBuilderDialogRowsFixtures.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include <vector>
#include "Menu.hpp"
#include "MenuBuilder.hpp"
#include "RecordingRowControl.hpp"

using rtype::client::Menu;
using rtype::client::MenuBuilder;

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
