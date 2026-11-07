#pragma once

#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include <cstddef>
#include <string_view>
#include <vector>
#include "FixedMasterAddress.hpp"
#include "MasterScreen.hpp"
#include "PlayerSettings.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingSettingsStorage.hpp"
#include "RecordingSoundPlayer.hpp"
#include "SettingsService.hpp"

/** @brief The form of the master alone, opened on FORM_MASTER_URL, over
 * settings that save to a recording storage, a recording navigator and a
 * recording canvas. */
class MasterScreenFixture : public ::testing::Test {
 protected:
  MasterScreenFixture();
  /** @brief Opens the form on the master @p url names. */
  explicit MasterScreenFixture(std::string_view url);

  [[nodiscard]] const RecordingNavigator& navigator() const;
  [[nodiscard]] const rtype::client::PlayerSettings& settings() const;
  /** @returns Every set of settings the form saved. */
  [[nodiscard]] const std::vector<rtype::client::PlayerSettings>& saves() const;
  [[nodiscard]] const InterfaceCanvasRecord& canvas() const;

  /** @brief Presses and releases a key. */
  void tap(sf::Keyboard::Key key);
  /** @brief Presses and releases a key, then lets the press last. */
  void choose(sf::Keyboard::Key key);
  /** @brief Types a text, one character after the other. */
  void type(std::u32string_view text);
  /** @brief Erases characters from the selected field. */
  void erase(std::size_t characters);

 private:
  RecordingNavigator navigator_;
  RecordingSoundPlayer sounds_;
  rtype::client::PlayerSettings settings_;
  RecordingSettingsStorage storage_;
  rtype::client::SettingsService service_;
  FixedMasterAddress master_;
  InterfaceCanvasRecord canvas_;
  rtype::client::MasterScreen screen_;
};

/** @brief The same form, opened on a master named by host. */
class MasterScreenForAHostFixture : public MasterScreenFixture {
 protected:
  MasterScreenForAHostFixture();
};
