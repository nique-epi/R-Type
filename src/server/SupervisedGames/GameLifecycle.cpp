#include "GameLifecycle.hpp"
#include <string_view>
#include "GameStatus.hpp"
#include "SupervisedGamesConstants.hpp"

namespace rtype::server {

std::string_view lifecycleText(GameLifecycle lifecycle) {
  switch (lifecycle) {
    case GameLifecycle::Starting:
      return STARTING_TEXT;
    case GameLifecycle::WaitingForPlayers:
      return WAITING_FOR_PLAYERS_TEXT;
    case GameLifecycle::InProgress:
      return IN_PROGRESS_TEXT;
    case GameLifecycle::Finished:
      return FINISHED_TEXT;
  }
  return STARTING_TEXT;
}

GameLifecycle lifecycleOf(network::GameStatusState state) {
  switch (state) {
    case network::GameStatusState::WaitingForPlayers:
      return GameLifecycle::WaitingForPlayers;
    case network::GameStatusState::InProgress:
      return GameLifecycle::InProgress;
    case network::GameStatusState::Finished:
      return GameLifecycle::Finished;
  }
  return GameLifecycle::WaitingForPlayers;
}

}  // namespace rtype::server
