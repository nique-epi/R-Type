#include "GameResultKeeper.hpp"
#include <chrono>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "GameOutcome.hpp"
#include "GameResult.hpp"
#include "GameSettings.hpp"
#include "GameState.hpp"
#include "GameStateChanged.hpp"
#include "GameTicket.hpp"
#include "IResultsArchive.hpp"
#include "PlayerRoster.hpp"
#include "ShipLookConstants.hpp"
#include "ShipLookNumbers.hpp"

namespace rtype::server {

namespace {

/** @returns The names of the items @p player won, in the order it won them. */
std::vector<std::string> wonLooksOf(const ConnectedPlayer& player) {
  std::vector<std::string> names;
  names.reserve(player.wonLookNumbers.size());
  for (const std::uint8_t number : player.wonLookNumbers) {
    names.emplace_back(
        identifiers::SHIP_LOOK_ITEM_NAMES.at(identifiers::lookIndexOf(number)));
  }
  return names;
}

/**
 * @returns What @p player leaves of the game: its deaths are the ships
 * LifeKeeper took from it (ConnectedPlayer::deaths), counted one by one, so a
 * checkpoint that gives the lives back takes none of them away.
 */
PlayerResult resultOf(const ConnectedPlayer& player) {
  return PlayerResult{.nickname = player.nickname,
                      .score = player.score,
                      .bounty = player.bounty,
                      .deaths = player.deaths,
                      .wonLooks = wonLooksOf(player),
                      .ticket = network::gameTicketText(player.ticket)};
}

}  // namespace

GameResultKeeper::GameResultKeeper(PlayerRoster& roster,
                                   IResultsArchive& archive,
                                   const GameSettings& settings)
    : roster_(roster),
      archive_(archive),
      difficulty_(settings.rules.difficulty),
      vanilla_(settings.vanilla) {}

void GameResultKeeper::onPlayerClosing(const ConnectedPlayer& departed) {
  if (state_ == GameState::InProgress) {
    departed_.push_back(resultOf(departed));
  }
}

void GameResultKeeper::onStateChanged(const GameStateChanged& change) {
  state_ = change.current;
  if (change.current == GameState::InProgress) {
    startCounting();
  } else if (change.previous == GameState::InProgress &&
             change.current == GameState::Finished) {
    recordTheGame(change.outcome == GameOutcome::Victory);
  }
}

void GameResultKeeper::startCounting() {
  playersAtStart_ = static_cast<std::uint8_t>(roster_.connected().size());
}

void GameResultKeeper::recordTheGame(bool victory) {
  std::vector<PlayerResult> players = std::exchange(departed_, {});
  for (const ConnectedPlayer* const player : roster_.connected()) {
    players.push_back(resultOf(*player));
  }
  if (players.empty()) {
    return;
  }
  archive_.record(
      GameResult{.difficulty = difficulty_,
                 .playerCount = playersAtStart_,
                 .endedAt = std::chrono::floor<std::chrono::seconds>(
                     std::chrono::system_clock::now()),
                 .players = std::move(players),
                 .vanilla = vanilla_,
                 .victory = victory});
}

}  // namespace rtype::server
