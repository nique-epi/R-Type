#include "RecordingSoundPlayer.hpp"
#include <string>
#include <string_view>
#include <vector>

void RecordingSoundPlayer::play(std::string_view soundId) {
  played_.emplace_back(soundId);
}

const std::vector<std::string>& RecordingSoundPlayer::played() const {
  return played_;
}

void RecordingSoundPlayer::load(std::string_view soundId) {
  loaded_.emplace_back(soundId);
}

const std::vector<std::string>& RecordingSoundPlayer::loaded() const {
  return loaded_;
}
