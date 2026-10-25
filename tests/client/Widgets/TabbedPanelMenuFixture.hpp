#pragma once

#include <gtest/gtest.h>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cstddef>
#include <vector>
#include "Menu.hpp"
#include "RecordingRowControl.hpp"
#include "RecordingSoundPlayer.hpp"

/** @brief A tabbed panel of three tabs: a row whose control steps on Left
 * and Right, a row whose control can be activated, then BACK. */
class TabbedPanelMenuFixture : public ::testing::Test {
 protected:
  TabbedPanelMenuFixture();

  void send(const sf::Event& event);

  /** @brief Presses and releases a key, then lets the press last. */
  void tap(sf::Keyboard::Key key,
           sf::Keyboard::Scancode scancode = sf::Keyboard::Scan::Unknown);

  rtype::client::Menu& menu();
  RowControlRecord& steps();
  RowControlRecord& link();
  [[nodiscard]] const std::vector<std::size_t>& opened() const;
  [[nodiscard]] int backs() const;

  /** @returns The sounds the menu played. */
  [[nodiscard]] const RecordingSoundPlayer& sounds() const;

 private:
  RowControlRecord steps_;
  RowControlRecord link_;
  std::vector<std::size_t> opened_;
  int backs_ = 0;
  RecordingSoundPlayer sounds_;
  rtype::client::Menu menu_;
};
