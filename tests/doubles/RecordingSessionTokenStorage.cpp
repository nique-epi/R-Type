#include "RecordingSessionTokenStorage.hpp"
#include <optional>
#include <string>

std::optional<std::string> RecordingSessionTokenStorage::load() const {
  return kept;
}

void RecordingSessionTokenStorage::keep(const std::string& token) {
  kept = token;
  ++keeps;
}

void RecordingSessionTokenStorage::forget() { kept.reset(); }
