#pragma once

#include <gtest/gtest.h>
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

  /** @brief Signs the account in as ACE, and lets a frame paint it. */
  void signInAsAce();

  /** @brief Selects the account bar from PLAY, going up past QUIT, and
   * activates it with Enter. */
  void chooseTheAccountBar();

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
