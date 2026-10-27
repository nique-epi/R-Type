#include "EndScreenDemo.hpp"
#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>
#include "EndScreenDemoConstants.hpp"
#include "FinalPilot.hpp"
#include "FinalRanking.hpp"
#include "GameResult.hpp"
#include "ProtocolConstants.hpp"

namespace rtype::client {

GameResult demoGameResult() {
  std::vector<FinalPilot> pilots;
  for (std::size_t index = 0; index < DEMO_NICKNAMES.size(); ++index) {
    const auto number =
        static_cast<std::uint8_t>(network::FIRST_PLAYER_NUMBER + index);
    pilots.push_back(FinalPilot{.playerNumber = number,
                                .nickname = std::string{DEMO_NICKNAMES[index]},
                                .score = DEMO_SCORES.at(index),
                                .isLocalPlayer = number == DEMO_LOCAL_PLAYER});
  }
  return GameResult{.pilots = rankedPilots(std::move(pilots))};
}

}  // namespace rtype::client
