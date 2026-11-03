#pragma once

#include <gtest/gtest.h>
#include <memory>
#include "FakeMusicPlayer.hpp"
#include "IntroScreen.hpp"
#include "PlayerSettings.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingSky.hpp"
#include "RecordingSoundPlayer.hpp"
#include "TimeConstants.hpp"

/** @brief An intro alone, every part a double: the test sets the settings
 * and the music, then starts it. */
class IntroScreenFixture : public ::testing::Test {
 protected:
  /** @brief Builds the intro from the doubles, as they are now. */
  void start();

  /** @returns The intro, once started. */
  rtype::client::IntroScreen& intro();

  /** @brief Updates the intro frame by frame, ONE_FRAME each, for
   * @p duration. */
  void playFor(rtype::engine::Duration duration);

  /** @brief Updates the intro frame by frame, ONE_FRAME each, for @p duration,
   * moving the music of the double by ONE_FRAME before each frame once it
   * plays, as the real music moves. */
  void playWithTheMusicFor(rtype::engine::Duration duration);

  /** @brief Ends the intro, as replacing it with the title does. */
  void close();

  [[nodiscard]] const RecordingNavigator& navigator() const;
  [[nodiscard]] const RecordingSky& sky() const;
  [[nodiscard]] const RecordingSoundPlayer& sounds() const;
  FakeMusicPlayer& music();
  rtype::client::PlayerSettings& settings();

 private:
  RecordingNavigator navigator_;
  rtype::client::PlayerSettings settings_;
  RecordingSky sky_;
  RecordingSoundPlayer sounds_;
  FakeMusicPlayer music_;
  InterfaceCanvasRecord canvas_;
  std::unique_ptr<rtype::client::IntroScreen> intro_;
};
