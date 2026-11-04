#include "LoadingScreenFixture.hpp"
#include <filesystem>
#include <memory>
#include <vector>
#include "AssetLibrary.hpp"
#include "AssetList.hpp"
#include "AssetTestConstants.hpp"
#include "IScreen.hpp"
#include "LoadingScreen.hpp"
#include "LoadingScreenParts.hpp"
#include "RecordingScreen.hpp"
#include "ScreenId.hpp"
#include "ScreenStack.hpp"
#include "ScreenTestConstants.hpp"

using rtype::client::AssetLibrary;
using rtype::client::AssetList;
using rtype::client::IScreen;
using rtype::client::LoadingScreen;
using rtype::client::LoadingScreenParts;
using rtype::client::ScreenId;
using rtype::client::ScreenStack;
using rtype::client::testing::BOSS_MUSIC_ID;
using rtype::client::testing::NOT_AN_ASSET;
using rtype::client::testing::STAGE_MUSIC_ID;

LoadingScreenFixture::LoadingScreenFixture()
    : library_(folder_.path()),
      screens_([this](ScreenId screen) { return buildScreen(screen); }) {}

void LoadingScreenFixture::loadTwoMusicFiles() {
  folder_.write(STAGE_MUSIC_ID, NOT_AN_ASSET);
  folder_.write(BOSS_MUSIC_ID, NOT_AN_ASSET);
  toLoad_ = AssetList{.music = {STAGE_MUSIC_ID, BOSS_MUSIC_ID}};
}

void LoadingScreenFixture::loadOnly(const AssetList& assets) {
  toLoad_ = assets;
}

void LoadingScreenFixture::showLoadingScreenOf(ScreenId screen) {
  screens_.replaceAll(screen);
  screens_.applyPendingChanges();
}

void LoadingScreenFixture::runFrame() {
  screens_.update(ONE_FRAME);
  screens_.applyPendingChanges();
}

ScreenStack& LoadingScreenFixture::screens() { return screens_; }

const AssetLibrary& LoadingScreenFixture::library() const { return library_; }

const std::filesystem::path& LoadingScreenFixture::folder() const {
  return folder_.path();
}

const std::vector<JournalEntry>& LoadingScreenFixture::journal() const {
  return journal_;
}

std::unique_ptr<IScreen> LoadingScreenFixture::buildScreen(ScreenId screen) {
  if (!loadingScreenBuilt_) {
    loadingScreenBuilt_ = true;
    return std::make_unique<LoadingScreen>(
        LoadingScreenParts{.navigator = screens_,
                           .library = library_,
                           .assets = toLoad_,
                           .nextScreen = screen});
  }
  return std::make_unique<RecordingScreen>(screen, journal_);
}
