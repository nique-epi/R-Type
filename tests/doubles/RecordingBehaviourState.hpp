#pragma once

#include <memory>
#include <vector>
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "IBehaviourState.hpp"
#include "IncomingHit.hpp"

namespace rtype::game::testing {

/**
 * @brief The state of a body whose behaviour answers every hit the same way,
 * and keeps the hits it was asked about, oldest first.
 */
class RecordingBehaviourState final : public IBehaviourState {
 public:
  /** @param letsHits What letsHit() answers. */
  explicit RecordingBehaviourState(bool letsHits);

  [[nodiscard]] bool letsHit(const IncomingHit& hit) override;

  /** @returns The hits letsHit() was asked about. */
  [[nodiscard]] const std::vector<IncomingHit>& askedHits() const;

 private:
  bool letsHits_;
  std::vector<IncomingHit> askedHits_;
};

/** @brief Gives @p body of @p components a Pilot whose behaviour keeps
 * @p state for it, a behaviour that asks nothing. */
void pilotKeeping(engine::ComponentRegistry& components, engine::Entity body,
                  std::shared_ptr<RecordingBehaviourState> state);

}  // namespace rtype::game::testing
