#pragma once

#include <gtest/gtest.h>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include <string>
#include "FixedMasterAddress.hpp"
#include "RecordingAudioService.hpp"
#include "RecordingGameCatalog.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingServerCatalog.hpp"
#include "RecordingSoundPlayer.hpp"
#include "ServerChoice.hpp"
#include "ServerListSnapshot.hpp"
#include "ServerMapScreen.hpp"
#include "ServerMapScreenTestConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client::testing {

/** @brief A server map over catalogs, a navigator and a canvas that record:
 * the tests set the lists, press keys and read what the map asked. */
class ServerMapScreenFixture : public ::testing::Test {
 protected:
  /** @brief Builds the map over @p list, as the catalog shows it, and runs a
   * frame. */
  void openOn(const ServerListSnapshot& list);
  /** @brief Presses and releases @p key, then runs a frame. */
  void press(sf::Keyboard::Key key);
  /** @brief Presses @p key without releasing it. */
  void hold(sf::Keyboard::Key key);
  /** @brief Moves the pointer to @p pixel of the interface, at the pixel of
   * the window INTERFACE_PIXEL times as far, when @p clicked is false;
   * presses and releases the left button there otherwise; then runs a frame. */
  void pointAt(sf::Vector2i pixel, bool clicked);
  void update(engine::Duration elapsed);

  [[nodiscard]] RecordingServerCatalog& servers();
  [[nodiscard]] RecordingGameCatalog& games();
  [[nodiscard]] const RecordingNavigator& navigator() const;
  [[nodiscard]] const RecordingAudioService& audio() const;
  [[nodiscard]] const ServerChoice& choice() const;
  [[nodiscard]] const InterfaceCanvasRecord& painting() const;

 private:
  RecordingNavigator navigator_;
  RecordingSoundPlayer sounds_;
  RecordingAudioService audio_{sounds_};
  RecordingServerCatalog servers_;
  RecordingGameCatalog games_;
  ServerChoice choice_;
  FixedMasterAddress master_{std::string{MAP_MASTER_URL}};
  InterfaceCanvasRecord painting_;
  std::unique_ptr<ServerMapScreen> screen_;
};

}  // namespace rtype::client::testing
