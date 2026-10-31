#include "DepartureFeedFixtures.hpp"
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include "DepartureFeed.hpp"
#include "InterfaceColorConstants.hpp"
#include "PilotStatus.hpp"
#include "RecordingInterfaceCanvas.hpp"

using rtype::client::DepartureFeed;
using rtype::client::PilotState;
using rtype::client::PilotStatus;

PilotStatus pilotIn(std::uint8_t number, std::string_view nickname,
                    PilotState state) {
  return PilotStatus{.playerNumber = number,
                     .nickname = std::string(nickname),
                     .score = 0,
                     .lives = std::nullopt,
                     .state = state,
                     .isLocalPlayer = false};
}

InterfaceCanvasRecord feedPaintingOf(const PilotStatus& pilot) {
  DepartureFeed feed;
  feed.follow(std::span{&pilot, 1});
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);
  feed.paint(canvas, rtype::client::PLAYER_COLORS);
  return record;
}
