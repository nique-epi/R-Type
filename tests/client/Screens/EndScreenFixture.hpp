#pragma once

#include <gtest/gtest.h>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cstddef>
#include <memory>
#include <optional>
#include "EndScreen.hpp"
#include "GameResult.hpp"
#include "MotionPreference.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingServerConnection.hpp"
#include "RecordingSoundPlayer.hpp"
#include "TimeConstants.hpp"

/** @brief An end screen after the game of the design system, its pointer
 * mapped one window pixel to one playfield unit. */
class EndScreenFixture : public ::testing::Test {
 protected:
  EndScreenFixture();

  /** @brief Builds the screen again, with this motion preference. */
  void rebuildWith(rtype::client::MotionPreference motion);
  /** @brief Presses then releases a key. */
  void tap(sf::Keyboard::Key key);
  /** @brief Clicks the left button on a pixel of the picture. */
  void click(sf::Vector2i interfacePixel);
  /** @brief Updates the screen frame by frame for @p duration. */
  void advance(rtype::engine::Duration duration);

  [[nodiscard]] rtype::client::EndScreen& screen();
  [[nodiscard]] const RecordingNavigator& navigator() const;
  [[nodiscard]] const RecordingServerConnection& connection() const;
  [[nodiscard]] const InterfaceCanvasRecord& canvas() const;
  [[nodiscard]] const RecordingSoundPlayer& sounds() const;
  [[nodiscard]] std::optional<std::size_t> previousTaunt() const;

 private:
  RecordingNavigator navigator_;
  RecordingServerConnection connection_;
  RecordingSoundPlayer sounds_;
  InterfaceCanvasRecord canvas_;
  rtype::client::GameResult result_;
  std::optional<std::size_t> previousTaunt_;
  std::unique_ptr<rtype::client::EndScreen> screen_;
};
