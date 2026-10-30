#pragma once

#include <cstddef>
#include "GameState.hpp"
#include "GameVacancy.hpp"

namespace rtype::server::testing {

/** @brief Indexes of games in a list handed to the placement of a player. */
constexpr std::size_t FIRST_GAME_INDEX = 0;
constexpr std::size_t SECOND_GAME_INDEX = 1;
constexpr std::size_t THIRD_GAME_INDEX = 2;

/** @brief Games as the placement sees them. */
constexpr GameVacancy WAITING_WITH_A_SEAT{.state = GameState::WaitingForPlayers,
                                          .full = false};
constexpr GameVacancy WAITING_AND_FULL{.state = GameState::WaitingForPlayers,
                                       .full = true};
constexpr GameVacancy IN_PROGRESS{.state = GameState::InProgress,
                                  .full = false};
constexpr GameVacancy FINISHED{.state = GameState::Finished, .full = false};

}  // namespace rtype::server::testing
