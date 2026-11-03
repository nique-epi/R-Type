#include "GamePlacement.hpp"
#include <cstddef>
#include <optional>
#include <span>
#include "GameState.hpp"
#include "GameVacancy.hpp"

namespace rtype::server {

std::optional<std::size_t> firstJoinableGame(
    std::span<const GameVacancy> games) {
  for (std::size_t index = 0; index < games.size(); ++index) {
    if (games[index].state == GameState::WaitingForPlayers &&
        !games[index].full) {
      return index;
    }
  }
  return std::nullopt;
}

}  // namespace rtype::server
