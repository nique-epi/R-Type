#pragma once

#include <gtest/gtest.h>
#include <memory>
#include <string_view>
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "WhatsNewScreen.hpp"

/** @brief WHAT'S NEW of two notes, with a recording navigator and canvas,
 * counting the times it marked the notes read. */
class WhatsNewScreenFixture : public ::testing::Test {
 protected:
  WhatsNewScreenFixture();

  /** @returns Whether the panel wrote @p text. */
  [[nodiscard]] bool wrote(std::string_view text) const;

  [[nodiscard]] const RecordingNavigator& navigator() const;
  [[nodiscard]] rtype::client::WhatsNewScreen& screen();
  [[nodiscard]] int closes() const;

 private:
  RecordingNavigator navigator_;
  InterfaceCanvasRecord canvas_;
  int closes_{};
  std::unique_ptr<rtype::client::WhatsNewScreen> screen_;
};
