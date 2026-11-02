#pragma once

#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include "LeaderboardPage.hpp"
#include "LeaderboardScreen.hpp"
#include "MotionPreference.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingLeaderboardSource.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingSoundPlayer.hpp"

/** @brief A leaderboard of a recording SERVER tab and a GLOBAL tab without a
 * source, opened on NORMAL and two pilots. */
class LeaderboardScreenFixture : public ::testing::Test {
 protected:
  explicit LeaderboardScreenFixture(rtype::client::MotionPreference motion =
                                        rtype::client::MotionPreference::Full);

  /** @brief Presses and releases a key. */
  void tap(sf::Keyboard::Key key);

  /** @brief The source shows this page, and a frame goes by. */
  void show(rtype::client::LeaderboardPage page);

  [[nodiscard]] const RecordingNavigator& navigator() const;
  [[nodiscard]] RecordingLeaderboardSource& source();
  [[nodiscard]] const InterfaceCanvasRecord& canvas() const;
  [[nodiscard]] rtype::client::LeaderboardScreen& screen();

  /** @brief Destroys the screen, as the stack does when it leaves. */
  void closeScreen();

 private:
  RecordingNavigator navigator_;
  RecordingSoundPlayer sounds_;
  RecordingLeaderboardSource source_;
  InterfaceCanvasRecord canvas_;
  std::unique_ptr<rtype::client::LeaderboardScreen> screen_;
};

/** @brief The same screen while the system asks for less motion. */
class StillLeaderboardFixture : public LeaderboardScreenFixture {
 protected:
  StillLeaderboardFixture();
};
