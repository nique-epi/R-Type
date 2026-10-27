#pragma once

#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include <cstddef>
#include "Dropdown.hpp"

/** @brief The list of window sizes on a 1920 x 1080 desktop, where the first
 * three sizes fit, on 1600 x 900. */
class DropdownFixture : public ::testing::Test {
 protected:
  DropdownFixture();

  /** @returns Whether the list used the key. */
  bool press(sf::Keyboard::Key key);

  [[nodiscard]] std::size_t size() const;
  rtype::client::Dropdown& list();

 private:
  std::size_t size_{};
  rtype::client::Dropdown list_;
};
