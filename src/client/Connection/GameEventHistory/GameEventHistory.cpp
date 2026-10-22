#include "GameEventHistory.hpp"
#include <algorithm>
#include <cstdint>
#include <vector>
#include "ConnectionConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

bool GameEventHistory::isFirstCopy(std::uint16_t eventNumber) {
  if (std::ranges::any_of(seen_, [eventNumber](const SeenEvent& seen) {
        return seen.eventNumber == eventNumber;
      })) {
    return false;
  }
  seen_.push_back(
      SeenEvent{.eventNumber = eventNumber, .age = engine::Duration::zero()});
  return true;
}

void GameEventHistory::advance(engine::Duration elapsed) {
  for (SeenEvent& seen : seen_) {
    seen.age += elapsed;
  }
  std::erase_if(seen_, [](const SeenEvent& seen) {
    return seen.age >= GAME_EVENT_MEMORY;
  });
}

void GameEventHistory::forget() { seen_.clear(); }

}  // namespace rtype::client
