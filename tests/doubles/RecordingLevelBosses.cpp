#include "RecordingLevelBosses.hpp"
#include <algorithm>
#include <cstddef>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "AnnouncedBoss.hpp"

namespace rtype::game::testing {

RecordingLevelBosses::RecordingLevelBosses(std::vector<std::string> bosses)
    : bosses_(std::move(bosses)) {}

bool RecordingLevelBosses::isBoss(std::string_view name) const {
  return std::ranges::find(bosses_, name) != bosses_.end();
}

void RecordingLevelBosses::announce(std::string_view name, int healthPercent) {
  announced_.push_back(
      AnnouncedBoss{.name = std::string{name}, .healthPercent = healthPercent});
}

std::size_t RecordingLevelBosses::pending() const {
  return announced_.size() - entered_;
}

void RecordingLevelBosses::enterAll() { entered_ = announced_.size(); }

std::span<const AnnouncedBoss> RecordingLevelBosses::announced() const {
  return announced_;
}

}  // namespace rtype::game::testing
