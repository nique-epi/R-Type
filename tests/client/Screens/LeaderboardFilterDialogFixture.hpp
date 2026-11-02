#pragma once

#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include <optional>
#include "LeaderboardFilterChoice.hpp"
#include "LeaderboardFilterDialog.hpp"
#include "RecordingSoundPlayer.hpp"

/** @brief A dialog open on the first difficulty and one pilot, which keeps what
 * APPLY gives it. */
class LeaderboardFilterDialogFixture : public ::testing::Test {
 protected:
  LeaderboardFilterDialogFixture();

  /** @brief Presses and releases a key, then lets the press last. */
  void choose(sf::Keyboard::Key key);

  [[nodiscard]] const rtype::client::LeaderboardFilterDialog& dialog() const;
  [[nodiscard]] const std::optional<rtype::client::LeaderboardFilterChoice>&
  applied() const;

 private:
  RecordingSoundPlayer sounds_;
  rtype::client::LeaderboardFilterDialog dialog_{sounds_};
  std::optional<rtype::client::LeaderboardFilterChoice> applied_;
};
