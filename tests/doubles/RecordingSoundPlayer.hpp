#pragma once

#include <string>
#include <string_view>
#include <vector>
#include "ISoundPlayer.hpp"

/** @brief Sound player double that writes down the id of every sound it is
 * asked to play, in order; it needs no audio device. */
class RecordingSoundPlayer : public rtype::client::ISoundPlayer {
 public:
  void play(std::string_view soundId) override;

  /** @returns The ids of the sounds played, the first played first. */
  [[nodiscard]] const std::vector<std::string>& played() const;

 private:
  std::vector<std::string> played_;
};
