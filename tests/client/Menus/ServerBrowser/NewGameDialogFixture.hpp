#pragma once

#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include <optional>
#include <string>
#include <vector>
#include "Endpoint.hpp"
#include "Menu.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingSoundPlayer.hpp"
#include "ServerChoice.hpp"

/** @brief The new game dialog of a server, over a recording navigator. */
class NewGameDialogFixture : public ::testing::Test {
 protected:
  /** @brief Builds the dialog of a server that lists @p difficulties. */
  void open(std::vector<std::string> difficulties);

  /** @brief Presses and releases @p key, then lets the press last. */
  void choose(sf::Keyboard::Key key);

  /** @returns What the dialog paints now. */
  [[nodiscard]] InterfaceCanvasRecord painting() const;

  [[nodiscard]] const RecordingNavigator& navigator() const;
  [[nodiscard]] const rtype::client::ServerChoice& choice() const;

  /** @returns The lives of the game CREATE asked for; 0 when it asked for
   * none, which no expected count is. */
  [[nodiscard]] unsigned int createdLives() const;

  /** @returns The server whose games the map listed, the one the dialog
   * opens a game on. */
  [[nodiscard]] static rtype::network::Endpoint listedServer();

 private:
  RecordingNavigator navigator_;
  RecordingSoundPlayer sounds_;
  rtype::client::ServerChoice choice_;
  std::optional<rtype::client::Menu> menu_;
};
