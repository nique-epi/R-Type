#include "RecordingBehaviourState.hpp"
#include <memory>
#include <optional>
#include <utility>
#include <vector>
#include "BehaviourView.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "IncomingHit.hpp"
#include "Pilot.hpp"
#include "ViewRecordingBehaviour.hpp"

namespace rtype::game::testing {

RecordingBehaviourState::RecordingBehaviourState(bool letsHits)
    : letsHits_(letsHits) {}

bool RecordingBehaviourState::letsHit(const IncomingHit& hit) {
  askedHits_.push_back(hit);
  return letsHits_;
}

const std::vector<IncomingHit>& RecordingBehaviourState::askedHits() const {
  return askedHits_;
}

void pilotKeeping(engine::ComponentRegistry& components, engine::Entity body,
                  std::shared_ptr<RecordingBehaviourState> state) {
  static std::optional<BehaviourView> seen;
  static const ViewRecordingBehaviour behaviour{seen};
  components.add(body, Pilot{.behaviour = behaviour,
                             .settings = {},
                             .behaviourState = std::move(state)});
}

}  // namespace rtype::game::testing
