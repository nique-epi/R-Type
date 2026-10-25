#pragma once

#include <gtest/gtest.h>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <string>
#include <vector>
#include "Menu.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingSoundPlayer.hpp"

/**
 * @brief A form whose first bar is a field of four bytes, NAME, starting with
 * AB, then a button, OK; their actions write their label in a journal.
 */
class MenuWithAFieldFixture : public ::testing::Test {
 protected:
  MenuWithAFieldFixture();

  rtype::client::Menu& menu();

  [[nodiscard]] const std::vector<std::string>& activated() const;

  void send(const sf::Event& event);

  /** @returns What the menu paints. */
  [[nodiscard]] InterfaceCanvasRecord painting() const;

  /** @returns The point of the playfield one pixel inside the line written
   * with this text, read from what the menu paints. */
  [[nodiscard]] sf::Vector2i pointOn(const std::string& text) const;

  /** @returns The sounds the menu played. */
  [[nodiscard]] const RecordingSoundPlayer& sounds() const;

 private:
  std::vector<std::string> activated_;
  RecordingSoundPlayer sounds_;
  rtype::client::Menu menu_;
};
