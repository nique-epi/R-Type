#pragma once

#include <gtest/gtest.h>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <string>
#include <vector>
#include "Menu.hpp"
#include "RecordingSoundPlayer.hpp"

/** @brief A page of three buttons, A, B and C, C being the cancel button;
 * their actions write their label in a journal. */
class MenuFixture : public ::testing::Test {
 protected:
  MenuFixture();

  rtype::client::Menu& menu();

  [[nodiscard]] const std::vector<std::string>& activated() const;

  void send(const sf::Event& event);

  /** @brief Presses and releases a key. */
  void tap(sf::Keyboard::Key key);

  /** @returns The point of the playfield at the top-left corner of the bar
   * labelled so. */
  sf::Vector2i barCorner(const std::string& label);

  /** @returns The sounds the menu played. */
  [[nodiscard]] const RecordingSoundPlayer& sounds() const;

 private:
  std::vector<std::string> activated_;
  RecordingSoundPlayer sounds_;
  rtype::client::Menu menu_;
};
