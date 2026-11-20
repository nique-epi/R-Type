#include "LevelMusic.hpp"
#include <memory>
#include <string>
#include "AssetLibrary.hpp"
#include "IMusicPlayer.hpp"
#include "MusicPlayer.hpp"
#include "MusicTrack.hpp"
#include "SettingsService.hpp"

namespace rtype::client {

LevelMusic::LevelMusic(SettingsService& settings) : settings_(&settings) {}

LevelMusic::~LevelMusic() {
  for (const auto& entry : players_) {
    settings_->unsubscribe(*entry.second);
  }
}

IMusicPlayer& LevelMusic::playerOf(const std::string& assetId,
                                   AssetLibrary& library) {
  const auto found = players_.find(assetId);
  if (found != players_.end()) {
    return *found->second;
  }
  const auto placed = players_.emplace(assetId, nullptr).first;
  placed->second = std::make_unique<MusicPlayer>(
      *settings_, MusicTrack{.assetId = placed->first.c_str()});
  settings_->subscribe(*placed->second);
  placed->second->open(library);
  return *placed->second;
}

}  // namespace rtype::client
