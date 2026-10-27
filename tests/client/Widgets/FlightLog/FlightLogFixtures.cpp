#include "FlightLogFixtures.hpp"
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include "FinalPilot.hpp"
#include "FinalRanking.hpp"
#include "FlightLogSchedule.hpp"
#include "FlightLogTestConstants.hpp"
#include "FlightLogTimeline.hpp"
#include "ProtocolConstants.hpp"

using rtype::client::FinalPilot;
using rtype::client::testing::BOTTOM_SCORE;
using rtype::client::testing::DESIGN_TAUNT_FIRST_LINE;
using rtype::client::testing::DESIGN_TAUNT_SECOND_LINE;
using rtype::client::testing::LONGEST_NICKNAME;
using rtype::client::testing::NICKNAME_LETTER;

std::vector<FinalPilot> pilotsScoring(
    const std::vector<std::uint32_t>& scores) {
  std::vector<FinalPilot> pilots;
  pilots.reserve(scores.size());
  for (std::size_t index = 0; index < scores.size(); ++index) {
    pilots.push_back(
        FinalPilot{.playerNumber = static_cast<std::uint8_t>(
                       rtype::network::FIRST_PLAYER_NUMBER + index),
                   .nickname = std::string(LONGEST_NICKNAME, NICKNAME_LETTER),
                   .score = scores[index],
                   .isLocalPlayer = index == 0});
  }
  return rtype::client::rankedPilots(pilots);
}

rtype::client::FlightLogSchedule designScheduleOf(std::size_t pilotCount) {
  const std::vector<std::string> taunt{std::string{DESIGN_TAUNT_FIRST_LINE},
                                       std::string{DESIGN_TAUNT_SECOND_LINE}};
  return rtype::client::flightLogScheduleOf(
      pilotsScoring(std::vector<std::uint32_t>(pilotCount, BOTTOM_SCORE)),
      taunt);
}
