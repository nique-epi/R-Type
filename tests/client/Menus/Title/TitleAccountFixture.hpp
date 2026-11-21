#pragma once

#include <gtest/gtest.h>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include "FakeAccountSession.hpp"
#include "FakeWallet.hpp"
#include "PlayerSettings.hpp"
#include "RecordingDeviceLinkFlow.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingSoundPlayer.hpp"
#include "SignInReturn.hpp"
#include "TitleScreen.hpp"

/** @brief A title screen alone, its account a guest until the test signs it
 * in, on a recording navigator, flow and canvas. */
class TitleAccountFixture : public ::testing::Test {
 protected:
  TitleAccountFixture();

  [[nodiscard]] const RecordingNavigator& navigator() const;
  [[nodiscard]] const InterfaceCanvasRecord& canvas() const;
  [[nodiscard]] FakeAccountSession& account();
  [[nodiscard]] const RecordingDeviceLinkFlow& deviceLinks() const;
  [[nodiscard]] const RecordingSoundPlayer& sounds() const;

  /** @brief Signs the account in as ACE, and lets a frame paint it. */
  void signInAsAce();

  /** @brief Presses @p key, without releasing it. */
  void press(sf::Keyboard::Key key);

  /** @brief Releases @p key, then lets the press of a bar run its action. */
  void release(sf::Keyboard::Key key);

  /** @brief Presses then releases the left button on @p pixel of the
   * interface picture, sent INTERFACE_PIXEL times larger to the window. */
  void clickAt(sf::Vector2i pixel);

 private:
  RecordingNavigator navigator_;
  InterfaceCanvasRecord canvas_;
  RecordingSoundPlayer sounds_;
  rtype::client::PlayerSettings settings_;
  FakeWallet wallet_;
  FakeAccountSession account_;
  RecordingDeviceLinkFlow deviceLinks_;
  rtype::client::SignInReturn signInReturn_;
  std::unique_ptr<rtype::client::TitleScreen> title_;
};
