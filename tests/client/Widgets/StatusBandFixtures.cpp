#include "StatusBandFixtures.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <algorithm>
#include <cstdint>
#include <iterator>
#include <optional>
#include <set>
#include <utility>
#include <vector>
#include "BandMoment.hpp"
#include "PilotStatus.hpp"
#include "PlayerPalette.hpp"
#include "PlayerPaletteConstants.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "StatusBand.hpp"
#include "StatusBandLook.hpp"
#include "StatusBandTestConstants.hpp"

using rtype::client::paintStatusBand;
using rtype::client::PilotState;
using rtype::client::PilotStatus;
using rtype::client::PlayerPalette;
using rtype::client::StatusBandLook;
using rtype::client::testing::SCORE;

PilotStatus flying(std::uint8_t number, std::optional<std::uint8_t> lives) {
  return PilotStatus{.playerNumber = number,
                     .nickname = "ACE",
                     .score = SCORE,
                     .lives = lives,
                     .state = PilotState::Flying,
                     .isLocalPlayer = false};
}

InterfaceCanvasRecord bandPaintingOf(std::vector<PilotStatus> pilots,
                                     const PlayerPalette& palette) {
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);
  paintStatusBand(canvas, pilots, StatusBandLook{.palette = palette});
  return record;
}

InterfaceCanvasRecord bandPaintingAt(std::vector<PilotStatus> pilots,
                                     BandMoment moment) {
  InterfaceCanvasRecord record;
  record.metrics = moment.metrics;
  RecordingInterfaceCanvas canvas(record);
  paintStatusBand(canvas, pilots,
                  StatusBandLook{.palette = rtype::client::PLAYER_COLORS,
                                 .sinceStart = moment.sinceStart,
                                 .flashes = moment.flashes});
  return record;
}

std::vector<RecordedImage> imagesOf(const InterfaceCanvasRecord& record,
                                    sf::IntRect source) {
  std::vector<RecordedImage> found;
  std::ranges::copy_if(
      record.images, std::back_inserter(found),
      [source](const RecordedImage& image) { return image.source == source; });
  return found;
}

std::set<std::pair<int, int>> pixelsIn(const InterfaceCanvasRecord& record,
                                       sf::Color color) {
  InterfaceCanvasRecord ofColor;
  std::ranges::copy_if(
      record.fills, std::back_inserter(ofColor.fills),
      [color](const RecordedFill& fill) { return fill.color == color; });
  return litPixelsOf(ofColor);
}
