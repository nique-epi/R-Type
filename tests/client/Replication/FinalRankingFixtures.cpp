#include "FinalRankingFixtures.hpp"
#include <cstdint>
#include <initializer_list>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include "FinalRankingTestConstants.hpp"
#include "GameEnded.hpp"
#include "PilotStatus.hpp"

using rtype::client::PilotStatus;
using rtype::client::testing::FIRST_NICKNAME;
using rtype::client::testing::FIRST_NUMBER;
using rtype::client::testing::SECOND_NICKNAME;
using rtype::client::testing::SECOND_NUMBER;
using rtype::client::testing::THIRD_NICKNAME;
using rtype::client::testing::THIRD_NUMBER;

std::vector<PilotStatus> boardOfThree() {
  return {PilotStatus{.playerNumber = FIRST_NUMBER,
                      .nickname = std::string{FIRST_NICKNAME},
                      .lives = std::nullopt},
          PilotStatus{.playerNumber = SECOND_NUMBER,
                      .nickname = std::string{SECOND_NICKNAME},
                      .lives = std::nullopt,
                      .isLocalPlayer = true},
          PilotStatus{.playerNumber = THIRD_NUMBER,
                      .nickname = std::string{THIRD_NICKNAME},
                      .lives = std::nullopt}};
}

rtype::network::GameEnded endedWith(
    std::initializer_list<std::pair<std::uint8_t, std::uint32_t>> scores) {
  rtype::network::GameEnded ended;
  for (const auto& [number, score] : scores) {
    ended.players.push_back({.playerNumber = number, .score = score});
  }
  return ended;
}
