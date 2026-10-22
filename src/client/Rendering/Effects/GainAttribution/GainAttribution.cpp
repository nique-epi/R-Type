#include "GainAttribution.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>
#include "EffectConstants.hpp"
#include "EntitySpriteConstants.hpp"
#include "Position.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

void GainAttribution::noteKill(game::Position place, bool big) {
  kills_.push_back(
      Kill{.place = place, .big = big, .age = engine::Duration::zero()});
}

void GainAttribution::noteScore(int pilotColor, std::uint32_t score) {
  if (!isPilotColor(pilotColor)) {
    return;
  }
  std::optional<std::uint32_t>& known =
      scores_[static_cast<std::size_t>(pilotColor)];
  if (known.has_value() && score > *known) {
    gains_.push_back(Gain{.pilotColor = pilotColor,
                          .points = score - *known,
                          .age = engine::Duration::zero()});
  }
  known = score;
}

void GainAttribution::advance(engine::Duration elapsed) {
  for (Kill& kill : kills_) {
    kill.age += elapsed;
  }
  for (Gain& gain : gains_) {
    gain.age += elapsed;
  }
  std::erase_if(kills_,
                [](const Kill& kill) { return kill.age >= GAIN_MATCH_WINDOW; });
  std::erase_if(gains_,
                [](const Gain& gain) { return gain.age >= GAIN_MATCH_WINDOW; });
}

std::optional<GainAttribution::Figure> GainAttribution::takeFigure() {
  if (kills_.empty() || gains_.empty()) {
    return std::nullopt;
  }
  const Figure figure{.place = kills_.back().place,
                      .points = totalPoints(),
                      .pilotColor = soleGainer(),
                      .big = std::ranges::any_of(kills_, &Kill::big)};
  kills_.clear();
  gains_.clear();
  return figure;
}

std::uint32_t GainAttribution::totalPoints() const {
  std::uint32_t points = 0;
  for (const Gain& gain : gains_) {
    points += gain.points;
  }
  return points;
}

std::optional<int> GainAttribution::soleGainer() const {
  const int first = gains_.front().pilotColor;
  const bool alone = kills_.size() == 1 &&
                     std::ranges::all_of(gains_, [first](const Gain& gain) {
                       return gain.pilotColor == first;
                     });
  return alone ? std::optional<int>(first) : std::nullopt;
}

}  // namespace rtype::client
