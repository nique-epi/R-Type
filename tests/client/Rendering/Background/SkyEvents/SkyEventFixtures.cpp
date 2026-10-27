#include "SkyEventFixtures.hpp"
#include <cmath>
#include <cstdint>
#include <random>
#include "ISkyEvent.hpp"
#include "RecordingPixelSurface.hpp"
#include "SkyEventTestConstants.hpp"

using rtype::client::ISkyEvent;

std::mt19937 generatorFrom(std::uint32_t seed) { return std::mt19937(seed); }

void advanceFor(ISkyEvent& event, float eventSeconds) {
  const auto frames = static_cast<int>(std::ceil(eventSeconds / FRAME));
  for (int frame = 0; frame < frames; ++frame) {
    event.advance(FRAME);
  }
}

RecordingPixelSurface paintOf(const ISkyEvent& event) {
  RecordingPixelSurface surface;
  event.paint(surface);
  return surface;
}
