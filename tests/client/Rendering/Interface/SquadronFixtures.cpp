#include "SquadronFixtures.hpp"
#include <SFML/System/Vector2.hpp>
#include <vector>
#include "PlayerPalette.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "Squadron.hpp"
#include "SquadronConstants.hpp"
#include "SquadronShip.hpp"
#include "SquadronTestConstants.hpp"
#include "TimeConstants.hpp"

using rtype::client::PlayerPalette;
using rtype::client::Squadron;
using rtype::client::SQUADRON_SHIPS;
using rtype::client::SquadronShip;
using rtype::client::testing::BOX;
using rtype::client::testing::FRAME;
using rtype::engine::Duration;

void play(Squadron& squadron, Duration total) {
  for (Duration played{}; played < total; played += FRAME) {
    squadron.advance(FRAME);
  }
}

InterfaceCanvasRecord paintingOf(const Squadron& squadron,
                                 const PlayerPalette& palette) {
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);
  squadron.paint(canvas, BOX.position, palette);
  return record;
}

std::vector<sf::Vector2i> cornersOf(const InterfaceCanvasRecord& record,
                                    sf::Vector2i size) {
  std::vector<sf::Vector2i> corners;
  for (const RecordedImage& image : record.images) {
    if (image.source.size == size) {
      corners.push_back(image.topLeft);
    }
  }
  return corners;
}

std::vector<sf::Vector2i> formationInBox() {
  std::vector<sf::Vector2i> places;
  places.reserve(SQUADRON_SHIPS.size());
  for (const SquadronShip& ship : SQUADRON_SHIPS) {
    places.push_back(BOX.position + sf::Vector2i(ship.place));
  }
  return places;
}
