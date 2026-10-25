#include "TabbedPanelMenuFixture.hpp"
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cstddef>
#include <memory>
#include <vector>
#include "KeyPress.hpp"
#include "Menu.hpp"
#include "MenuBuilder.hpp"
#include "PointerEvents.hpp"
#include "RecordingRowControl.hpp"
#include "RecordingSoundPlayer.hpp"
#include "WidgetConstants.hpp"

using rtype::client::Menu;
using rtype::client::MenuBuilder;

TabbedPanelMenuFixture::TabbedPanelMenuFixture()
    : menu_(MenuBuilder(MenuBuilder::Frame::TabbedPanel, &sounds_)
                .heading("SETTINGS")
                .tabs({"A", "B", "C"}, 0, {.previous = "Q", .next = "E"},
                      [this](std::size_t tab) { opened_.push_back(tab); })
                .row({.label = "STEPS"},
                     std::make_unique<RecordingRowControl>(
                         steps_,
                         std::vector{sf::Keyboard::Key::Left,
                                     sf::Keyboard::Key::Right},
                         false))
                .row({.label = "LINK"},
                     std::make_unique<RecordingRowControl>(
                         link_, std::vector<sf::Keyboard::Key>{}, true))
                .cancelButton({.label = "BACK"}, [this] { ++backs_; })
                .build()) {}

void TabbedPanelMenuFixture::send(const sf::Event& event) {
  menu_.handleEvent(event, samePlayfieldPoint);
}

void TabbedPanelMenuFixture::tap(sf::Keyboard::Key key,
                                 sf::Keyboard::Scancode scancode) {
  send(keyPress(key, scancode));
  send(keyRelease(key, scancode));
  menu_.advance(rtype::client::PRESS_DURATION);
}

Menu& TabbedPanelMenuFixture::menu() { return menu_; }

RowControlRecord& TabbedPanelMenuFixture::steps() { return steps_; }

RowControlRecord& TabbedPanelMenuFixture::link() { return link_; }

const std::vector<std::size_t>& TabbedPanelMenuFixture::opened() const {
  return opened_;
}

int TabbedPanelMenuFixture::backs() const { return backs_; }

const RecordingSoundPlayer& TabbedPanelMenuFixture::sounds() const {
  return sounds_;
}
