#include "EmissionGroup.hpp"
#include <cstddef>
#include <cstdint>
#include <random>
#include <utility>
#include <vector>
#include "DurationRange.hpp"
#include "EmissionGroupDescription.hpp"
#include "EmissionGroupMember.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The weight of each of @p members, in their order. */
std::vector<double> weightsOf(const std::vector<EmissionGroupMember>& members) {
  std::vector<double> weights;
  weights.reserve(members.size());
  for (const EmissionGroupMember& member : members) {
    weights.push_back(member.weight);
  }
  return weights;
}

/** @returns A duration of @p range, drawn from @p random. */
engine::Duration drawnIn(const DurationRange& range, std::mt19937& random) {
  std::uniform_int_distribution<engine::Duration::rep> durations(
      range.shortest.count(), range.longest.count());
  return engine::Duration{durations(random)};
}

}  // namespace

EmissionGroup::EmissionGroup(const EmissionGroupDescription& description,
                             std::vector<EmissionGroupMember> members,
                             std::uint32_t seed)
    : description_(description),
      members_(std::move(members)),
      random_(seed),
      untilNextBirth_(description.first) {
  const std::vector<double> weights = weightsOf(members_);
  ranks_ =
      std::discrete_distribution<std::size_t>(weights.begin(), weights.end());
}

void EmissionGroup::advance(engine::Duration elapsed) {
  untilNextBirth_ -= elapsed;
  if (untilNextBirth_ > engine::Duration::zero() || members_.empty() ||
      aliveCount() >= description_.mostAlive) {
    return;
  }
  members_[ranks_(random_)].emitter.get().emit();
  untilNextBirth_ = drawnIn(description_.every, random_);
}

std::size_t EmissionGroup::aliveCount() const {
  std::size_t alive = 0;
  for (const EmissionGroupMember& member : members_) {
    alive += member.emitter.get().aliveCount();
  }
  return alive;
}

}  // namespace rtype::client
