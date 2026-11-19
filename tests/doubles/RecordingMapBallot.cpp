#include "RecordingMapBallot.hpp"
#include <cstdint>
#include <optional>
#include <vector>

void RecordingMapBallot::voteFor(std::uint8_t map) { votes_.push_back(map); }

std::optional<std::uint8_t> RecordingMapBallot::requestedMap() const {
  if (votes_.empty()) {
    return std::nullopt;
  }
  return votes_.back();
}

const std::vector<std::uint8_t>& RecordingMapBallot::votes() const {
  return votes_;
}
