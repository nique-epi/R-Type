#include "LeaderboardPage.hpp"
#include "LeaderboardPhase.hpp"

namespace rtype::client {

void enterPhase(LeaderboardPage& page, LeaderboardPhase phase) {
  page.rows.clear();
  page.ownRow.reset();
  page.phase = phase;
  ++page.revision;
}

}  // namespace rtype::client
