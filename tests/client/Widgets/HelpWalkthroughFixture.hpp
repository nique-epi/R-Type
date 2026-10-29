#pragma once

#include <gtest/gtest.h>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cstddef>
#include <memory>
#include <optional>
#include "BoundAction.hpp"
#include "HelpWalkthrough.hpp"
#include "KeyBindings.hpp"
#include "MotionPreference.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingSoundPlayer.hpp"

/** @brief A help built as the room builds it, painted on a recording canvas
 * with the parts of a room. */
class HelpWalkthroughFixture : public ::testing::Test {
 protected:
  HelpWalkthroughFixture();

  /** @brief Builds the help again, closed, with the keys bound so far and
   * that motion: the help names its keys once, when built. */
  void rebuild(rtype::client::MotionPreference motion);
  /** @brief Binds a key to an action, then builds the help again. */
  void bind(rtype::client::BoundAction action, sf::Keyboard::Scancode key);
  /** @returns Whether the help took the press of the key. */
  bool press(sf::Keyboard::Key key);
  /** @returns Whether the help took the left click at that pixel of the
   * window. */
  bool click(sf::Vector2i windowPixel);
  /** @brief Opens the help with H, then shows that step with Enter. */
  void showStep(std::size_t step);
  /** @returns What the help paints now. */
  const InterfaceCanvasRecord& painted();
  /** @returns The image of the professor the help paints now, if any. */
  [[nodiscard]] std::optional<RecordedImage> professorImage();
  rtype::client::HelpWalkthrough& help();
  [[nodiscard]] const RecordingSoundPlayer& sounds() const;

 private:
  RecordingSoundPlayer sounds_;
  rtype::client::KeyBindings keys_;
  InterfaceCanvasRecord record_;
  RecordingInterfaceCanvas canvas_{record_};
  std::unique_ptr<rtype::client::HelpWalkthrough> help_;
};
