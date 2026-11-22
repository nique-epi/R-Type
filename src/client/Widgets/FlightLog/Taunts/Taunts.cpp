#include "Taunts.hpp"
#include <algorithm>
#include <cstddef>
#include <optional>
#include <random>
#include <ranges>
#include <span>
#include <string>
#include <utility>
#include <vector>
#include "FinalPilot.hpp"
#include "Language.hpp"
#include "PilotName.hpp"
#include "Taunt.hpp"
#include "TauntCondition.hpp"
#include "TauntConstants.hpp"
#include "TextMark.hpp"
#include "TextMarks.hpp"

namespace rtype::client {

namespace {

bool scoredZero(const FinalPilot& pilot) { return pilot.score == 0; }

/** @returns A line of a taunt, its marks replaced by the names of the
 * pilots. */
std::string withNames(std::string line, std::span<const FinalPilot> pilots) {
  if (pilots.empty()) {
    return line;
  }
  line = withMark(std::move(line),
                  TextMark{.mark = BEST_PILOT_MARK,
                           .value = pilotNameOf(pilots.front().nickname,
                                                pilots.front().playerNumber)});
  line = withMark(std::move(line),
                  TextMark{.mark = LAST_PILOT_MARK,
                           .value = pilotNameOf(pilots.back().nickname,
                                                pilots.back().playerNumber)});
  const auto zero = std::ranges::find_if(pilots, scoredZero);
  if (zero != pilots.end()) {
    line = withMark(
        std::move(line),
        TextMark{.mark = ZERO_PILOT_MARK,
                 .value = pilotNameOf(zero->nickname, zero->playerNumber)});
  }
  return line;
}

}  // namespace

bool tauntFits(const Taunt& taunt, std::span<const FinalPilot> pilots) {
  switch (taunt.condition) {
    case TauntCondition::SeveralPilots:
      return pilots.size() > 1;
    case TauntCondition::BestAheadOfLast:
      return pilots.size() > 1 && pilots.front().score > pilots.back().score;
    case TauntCondition::ZeroScore:
      return std::ranges::any_of(pilots, scoredZero);
    case TauntCondition::Solo:
      return pilots.size() == 1;
    case TauntCondition::Always:
      break;
  }
  return true;
}

std::size_t pickTaunt(std::span<const FinalPilot> pilots, std::mt19937& random,
                      std::optional<std::size_t> previous) {
  std::vector<std::size_t> candidates;
  for (std::size_t index = 0; index < TAUNTS.size(); ++index) {
    if (previous != index && tauntFits(TAUNTS[index], pilots)) {
      candidates.push_back(index);
    }
  }
  std::uniform_int_distribution<std::size_t> pick(0, candidates.size() - 1);
  return candidates[pick(random)];
}

std::vector<std::string> tauntLinesOf(const Taunt& taunt,
                                      std::span<const FinalPilot> pilots,
                                      Language language) {
  std::vector<std::string> lines;
  for (const auto line :
       std::views::split(taunt.text.in(language), TAUNT_LINE_BREAK)) {
    lines.push_back(withNames(std::string(line.begin(), line.end()), pilots));
  }
  return lines;
}

}  // namespace rtype::client
