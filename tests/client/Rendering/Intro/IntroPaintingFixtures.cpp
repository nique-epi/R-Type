#include "IntroPaintingFixtures.hpp"
#include <SFML/Graphics/Color.hpp>
#include <algorithm>
#include <cstddef>
#include <span>
#include <string_view>
#include "FlashPreference.hpp"
#include "IntroCast.hpp"
#include "IntroCastBuilder.hpp"
#include "IntroFrame.hpp"
#include "IntroFreeze.hpp"
#include "IntroPainting.hpp"
#include "IntroTestConstants.hpp"
#include "IntroTimeline.hpp"
#include "IntroTimelineConstants.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "SquadronTestConstants.hpp"

using rtype::client::FlashPreference;
using rtype::client::IntroFrame;
using rtype::client::IntroFreeze;

InterfaceCanvasRecord paintedAt(float real, FlashPreference preference) {
  const rtype::client::IntroCast cast =
      rtype::client::buildIntroCast(rtype::client::testing::INTRO_TEST_SEED);
  const std::span<const IntroFreeze> freezes =
      preference == FlashPreference::Full
          ? std::span<const IntroFreeze>(rtype::client::INTRO_FREEZES)
          : std::span<const IntroFreeze>();
  const float scene = rtype::client::sceneTimeAt(real, freezes);
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);
  rtype::client::paintIntroCast(
      canvas,
      IntroFrame{
          .cast = cast,
          .real = real,
          .scene = scene,
          .view = rtype::client::cinemaViewAt(real),
          .shake = {},
          .drift = rtype::client::debrisDriftAt(scene),
          .preference = preference,
          .squadronBox = rtype::client::testing::SQUADRON_BOX_ON_THE_TITLE});
  return record;
}

std::size_t imagesOnRow(const InterfaceCanvasRecord& record,
                        std::string_view assetId, int top) {
  return static_cast<std::size_t>(
      std::ranges::count_if(record.images, [&](const RecordedImage& image) {
        return image.assetId == assetId && image.source.position.y == top;
      }));
}

std::size_t fillsColored(const InterfaceCanvasRecord& record, sf::Color color) {
  return static_cast<std::size_t>(
      std::ranges::count(record.fills, color, &RecordedFill::color));
}

std::size_t imagesTinted(const InterfaceCanvasRecord& record, sf::Color tint) {
  return static_cast<std::size_t>(
      std::ranges::count(record.images, tint, &RecordedImage::tint));
}

std::size_t speedLinesOf(const InterfaceCanvasRecord& record) {
  return static_cast<std::size_t>(
      std::ranges::count_if(record.fills, [](const RecordedFill& fill) {
        return fill.area.size.y == 1 &&
               fill.area.size.x >= rtype::client::testing::SHORTEST_SPEED_LINE;
      }));
}
