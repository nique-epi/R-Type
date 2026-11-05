#include "GameProcessWatch.hpp"
#include <cstddef>
#include <optional>
#include <string_view>
#include "SupervisorLinkConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

void GameProcessWatch::tick(engine::Duration elapsed, std::size_t playerCount) {
  sinceSupervisor_ += elapsed;
  if (playerCount > 0) {
    hadPlayers_ = true;
    withoutPlayers_ = engine::Duration::zero();
    return;
  }
  withoutPlayers_ += elapsed;
}

void GameProcessWatch::heardSupervisor() {
  sinceSupervisor_ = engine::Duration::zero();
}

std::optional<std::string_view> GameProcessWatch::reasonToStop() const {
  if (sinceSupervisor_ >= SUPERVISOR_SILENCE_LIMIT) {
    return SUPERVISOR_SILENT_REASON;
  }
  if (hadPlayers_ && withoutPlayers_ >= EMPTY_GAME_GRACE) {
    return EMPTY_GAME_REASON;
  }
  if (!hadPlayers_ && withoutPlayers_ >= IDLE_GAME_TIMEOUT) {
    return IDLE_GAME_REASON;
  }
  return std::nullopt;
}

}  // namespace rtype::server
