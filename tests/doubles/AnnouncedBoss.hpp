#pragma once

#include <string>

namespace rtype::game::testing {

/** @brief A boss a level announced to RecordingLevelBosses: its name and its
 * health percent. */
struct AnnouncedBoss {
  std::string name;
  int healthPercent{};

  bool operator==(const AnnouncedBoss&) const = default;
};

}  // namespace rtype::game::testing
