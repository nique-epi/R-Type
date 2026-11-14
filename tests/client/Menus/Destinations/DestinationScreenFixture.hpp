#pragma once

#include <gtest/gtest.h>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include "DestinationScreen.hpp"
#include "PlayStart.hpp"
#include "PlayerSettings.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingSoundPlayer.hpp"

/** @brief A destination screen alone, with a recording navigator, canvas and
 * sound player, and a balance of BALANCE kopecks. */
class DestinationScreenFixture : public ::testing::Test {
 protected:
  DestinationScreenFixture();

  /** @brief Builds a new screen in place of the one shown, SERVER leading
   * where @p playStart says. */
  void reopenWith(rtype::client::PlayStart playStart);

  /** @brief Presses and releases a key, then lets a frame pass. */
  void press(sf::Keyboard::Key key);

  /** @brief Presses the left button over @p interfacePixel, a pixel of the
   * interface picture, then lets a frame pass. */
  void clickAt(sf::Vector2i interfacePixel);

  rtype::client::DestinationScreen& screen();
  /** @brief The settings the screen reads its balance from. */
  rtype::client::PlayerSettings& settings();
  [[nodiscard]] const RecordingNavigator& navigator() const;
  [[nodiscard]] const InterfaceCanvasRecord& canvas() const;
  [[nodiscard]] const RecordingSoundPlayer& sounds() const;

 private:
  RecordingNavigator navigator_;
  InterfaceCanvasRecord canvas_;
  RecordingSoundPlayer sounds_;
  rtype::client::PlayerSettings settings_;
  std::unique_ptr<rtype::client::DestinationScreen> screen_;
};
