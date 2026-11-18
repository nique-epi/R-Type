#pragma once

#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include <string>
#include <vector>
#include "FakeAccountSession.hpp"
#include "RecordingDeviceLinkFlow.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingSoundPlayer.hpp"
#include "SignInReturn.hpp"
#include "SignInScreen.hpp"

/** @brief A sign-in dialog alone, on a recording navigator, flow and canvas,
 * its clock at SIGN_IN_NOW, the pages it opens written down. */
class SignInScreenFixture : public ::testing::Test {
 protected:
  SignInScreenFixture();

  [[nodiscard]] const RecordingNavigator& navigator() const;
  [[nodiscard]] RecordingDeviceLinkFlow& deviceLinks();
  [[nodiscard]] FakeAccountSession& account();
  [[nodiscard]] rtype::client::SignInReturn& signInReturn();
  [[nodiscard]] const std::vector<std::string>& openedPages() const;

  /** @brief Lets one frame of PRESS_DURATION pass. */
  void frame();

  /** @brief Presses and releases a key, then lets the press last. */
  void choose(sf::Keyboard::Key key);

 private:
  RecordingNavigator navigator_;
  RecordingDeviceLinkFlow deviceLinks_;
  FakeAccountSession account_;
  rtype::client::SignInReturn signInReturn_;
  RecordingSoundPlayer sounds_;
  InterfaceCanvasRecord canvas_;
  std::vector<std::string> openedPages_;
  std::unique_ptr<rtype::client::SignInScreen> screen_;
};
