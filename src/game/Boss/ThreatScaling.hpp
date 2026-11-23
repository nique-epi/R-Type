#pragma once

namespace rtype::game {

/** @brief What a share is raised by for the threat of a team: the percent the
 * most threat adds, and the threat in percent of the most. */
struct ThreatScaling {
  int percentAtTheMost{};
  int threatPercent{};
};

}  // namespace rtype::game
