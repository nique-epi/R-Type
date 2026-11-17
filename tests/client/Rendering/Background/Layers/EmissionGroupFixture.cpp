#include "EmissionGroupFixture.hpp"
#include <cstddef>
#include <string>
#include <utility>
#include <vector>
#include "CountingEmitter.hpp"
#include "DurationRange.hpp"
#include "EmissionGroup.hpp"
#include "EmissionGroupDescription.hpp"
#include "EmissionGroupMember.hpp"
#include "EmissionGroupTestConstants.hpp"
#include "LayerTestConstants.hpp"

using rtype::client::DurationRange;
using rtype::client::EmissionGroup;
using rtype::client::EmissionGroupDescription;
using rtype::client::EmissionGroupMember;

namespace {

EmissionGroupDescription skyEventDescription() {
  return EmissionGroupDescription{
      .name = std::string{EVENT_GROUP_NAME},
      .mostAlive = MOST_EVENTS_AT_ONCE,
      .first = FIRST_EVENT_DELAY,
      .every = DurationRange{.shortest = SHORTEST_EVENT_GAP,
                             .longest = LONGEST_EVENT_GAP}};
}

}  // namespace

EmissionGroup EmissionGroupFixture::skyEvents() {
  std::vector<EmissionGroupMember> members{
      EmissionGroupMember{.emitter = likely_,
                          .weight = WEIGHT_OF_THE_LIKELY_MEMBER},
      EmissionGroupMember{.emitter = rare_,
                          .weight = WEIGHT_OF_THE_RARE_MEMBER}};
  return EmissionGroup{skyEventDescription(), std::move(members), LAYER_SEED};
}

EmissionGroup EmissionGroupFixture::skyEventsWithoutMembers() {
  return EmissionGroup{skyEventDescription(), {}, LAYER_SEED};
}

CountingEmitter& EmissionGroupFixture::likely() { return likely_; }

CountingEmitter& EmissionGroupFixture::rare() { return rare_; }

std::size_t EmissionGroupFixture::births() const {
  return likely_.births() + rare_.births();
}
