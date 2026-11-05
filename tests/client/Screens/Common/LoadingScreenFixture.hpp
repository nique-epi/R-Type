#pragma once

#include <gtest/gtest.h>
#include <filesystem>
#include <memory>
#include <vector>
#include "AssetLibrary.hpp"
#include "AssetList.hpp"
#include "IScreen.hpp"
#include "RecordingScreen.hpp"
#include "ScreenId.hpp"
#include "ScreenStack.hpp"
#include "TemporaryFolder.hpp"

/** @brief A stack whose first screen is a loading screen for the assets the
 * test chose, and whose next screens are recording doubles. */
class LoadingScreenFixture : public ::testing::Test {
 protected:
  LoadingScreenFixture();

  /** @brief Writes two music files and makes them what the loading screen
   * loads. */
  void loadTwoMusicFiles();

  /** @brief Makes @p assets what the loading screen loads, written or not. */
  void loadOnly(const rtype::client::AssetList& assets);

  /** @brief Opens the loading screen of @p screen. */
  void showLoadingScreenOf(rtype::client::ScreenId screen);

  /** @brief Updates the stack by one frame and applies what it asked. */
  void runFrame();

  rtype::client::ScreenStack& screens();
  [[nodiscard]] const rtype::client::AssetLibrary& library() const;
  [[nodiscard]] const std::filesystem::path& folder() const;
  [[nodiscard]] const std::vector<JournalEntry>& journal() const;

 private:
  /** @returns The loading screen the first time, a recording screen after. */
  [[nodiscard]] std::unique_ptr<rtype::client::IScreen> buildScreen(
      rtype::client::ScreenId screen);

  TemporaryFolder folder_;
  rtype::client::AssetLibrary library_;
  rtype::client::AssetList toLoad_;
  bool loadingScreenBuilt_ = false;
  std::vector<JournalEntry> journal_;
  rtype::client::ScreenStack screens_;
};
