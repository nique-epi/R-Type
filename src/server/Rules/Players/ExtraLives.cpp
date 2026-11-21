#include "ExtraLives.hpp"
#include <cstdint>
#include "LifeKeeperConstants.hpp"
#include "ScoreChange.hpp"

namespace rtype::server {

namespace {

/** @returns How many steps of the extra lives @p score reached. */
std::uint32_t stepsReachedBy(std::uint32_t score) {
  if (score < FIRST_EXTRA_LIFE_SCORE) {
    return 0;
  }
  return 1 + ((score - FIRST_EXTRA_LIFE_SCORE) / EXTRA_LIFE_SCORE_STEP);
}

}  // namespace

std::uint32_t extraLivesWonBy(ScoreChange change) {
  if (change.after <= change.before) {
    return 0;
  }
  return stepsReachedBy(change.after) - stepsReachedBy(change.before);
}

}  // namespace rtype::server
