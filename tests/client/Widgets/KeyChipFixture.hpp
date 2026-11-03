#pragma once

#include <gtest/gtest.h>
#include "KeyChip.hpp"
#include "RecordingSoundPlayer.hpp"

/** @brief A chip on CHIP_AREA that counts how often its action runs. */
class KeyChipFixture : public ::testing::Test {
 protected:
  KeyChipFixture();

  /** @brief Gives an event to the chip. @returns Whether it was the chip's. */
  bool give(const sf::Event& event);

  [[nodiscard]] int activations() const;
  [[nodiscard]] const RecordingSoundPlayer& sounds() const;
  [[nodiscard]] rtype::client::KeyChip& chip();

 private:
  RecordingSoundPlayer sounds_;
  int activations_ = 0;
  rtype::client::KeyChip chip_;
};
