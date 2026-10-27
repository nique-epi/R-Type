#pragma once

#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include "Dropdown.hpp"

/** @brief The list of window sizes on a 1920 x 1080 desktop, where the first
 * three sizes fit, on 1600 x 900; no text is imposed until impose(). */
class DropdownFixture : public ::testing::Test {
 protected:
  DropdownFixture();

  /** @returns Whether the list used the key. */
  bool press(sf::Keyboard::Key key);

  [[nodiscard]] std::size_t size() const;
  rtype::client::Dropdown& list();

  /** @brief Makes the box show this text in place of the current size. */
  void impose(std::string_view text);

 private:
  std::size_t size_{};
  std::optional<std::string> imposed_;
  rtype::client::Dropdown list_;
};
