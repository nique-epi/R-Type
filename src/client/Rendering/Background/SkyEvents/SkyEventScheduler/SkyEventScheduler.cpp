#include "SkyEventScheduler.hpp"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <random>
#include <vector>
#include "Asteroid.hpp"
#include "Comet.hpp"
#include "Flare.hpp"
#include "IPixelSurface.hpp"
#include "ISkyEvent.hpp"
#include "RandomRange.hpp"
#include "ShootingStar.hpp"
#include "SkyEventConstants.hpp"
#include "SkyEventKind.hpp"

namespace rtype::client {

SkyEventScheduler::SkyEventScheduler(std::uint32_t seed)
    : random_(seed),
      kinds_(SKY_EVENT_WEIGHTS.begin(), SKY_EVENT_WEIGHTS.end()) {
  events_.reserve(MAXIMUM_SKY_EVENTS);
}

void SkyEventScheduler::advance(float seconds) {
  secondsUntilNextEvent_ -= seconds;
  if (secondsUntilNextEvent_ <= 0.0F && events_.size() < MAXIMUM_SKY_EVENTS) {
    events_.push_back(createEvent(static_cast<SkyEventKind>(kinds_(random_))));
    secondsUntilNextEvent_ =
        randomBetween(random_, SHORTEST_SKY_EVENT_GAP, LONGEST_SKY_EVENT_GAP);
  }
  const float eventSeconds = seconds * SKY_EVENT_CLOCK_RATE;
  for (const std::unique_ptr<ISkyEvent>& event : events_) {
    event->advance(eventSeconds);
  }
  std::erase_if(events_, [](const std::unique_ptr<ISkyEvent>& event) {
    return event->isOver();
  });
}

void SkyEventScheduler::paint(IPixelSurface& surface) const {
  for (const std::unique_ptr<ISkyEvent>& event : events_) {
    event->paint(surface);
  }
}

std::size_t SkyEventScheduler::eventCount() const { return events_.size(); }

std::unique_ptr<ISkyEvent> SkyEventScheduler::createEvent(SkyEventKind kind) {
  switch (kind) {
    case SkyEventKind::ShootingStar:
      return std::make_unique<ShootingStar>(random_);
    case SkyEventKind::Flare:
      return std::make_unique<Flare>(random_);
    case SkyEventKind::Asteroid:
      return std::make_unique<Asteroid>(random_);
    case SkyEventKind::Comet:
      return std::make_unique<Comet>(random_);
  }
  return std::make_unique<ShootingStar>(random_);
}

}  // namespace rtype::client
