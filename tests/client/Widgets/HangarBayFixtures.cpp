#include "HangarBayFixtures.hpp"
#include <SFML/Graphics/Color.hpp>
#include <algorithm>
#include <cstddef>
#include <optional>
#include "HangarBay.hpp"
#include "HangarBayTestConstants.hpp"
#include "InterfaceColorConstants.hpp"
#include "MotionPreference.hpp"
#include "PilotTestConstants.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "TimeConstants.hpp"

using rtype::client::BayMotion;
using rtype::client::BayPilot;
using rtype::client::BayReadiness;
using rtype::client::HangarBay;
using rtype::client::MotionPreference;
using rtype::client::PLAYER_COLORS;
using rtype::client::testing::BAY_TOP_LEFT;
using rtype::client::testing::BEACON_SQUARE;
using rtype::client::testing::NICKNAME;
using rtype::client::testing::THIRD_PLAYER;

HangarBay bayOf(std::optional<BayReadiness> readiness, bool isLocalPlayer) {
  HangarBay bay{.topLeft = BAY_TOP_LEFT,
                .playerNumber = THIRD_PLAYER,
                .color = THIRD_PILOT_COLOR,
                .palette = PLAYER_COLORS,
                .pilot = std::nullopt};
  if (readiness.has_value()) {
    bay.pilot = BayPilot{.nickname = NICKNAME,
                         .readiness = *readiness,
                         .isLocalPlayer = isLocalPlayer};
  }
  return bay;
}

InterfaceCanvasRecord bayPaintingOf(const HangarBay& bay,
                                    rtype::engine::Duration elapsed,
                                    MotionPreference motion) {
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);
  rtype::client::paintHangarBay(
      canvas, bay, BayMotion{.elapsed = elapsed, .preference = motion});
  return record;
}

std::size_t beaconsOfColor(const InterfaceCanvasRecord& record,
                           sf::Color color) {
  return static_cast<std::size_t>(std::ranges::count_if(
      record.fills, [color](const RecordedFill& fill) noexcept {
        return fill.color == color && fill.area.size == BEACON_SQUARE;
      }));
}
