#include "PaintedBeamsScene.hpp"
#include <cstdint>
#include <optional>
#include "AppearanceReference.hpp"
#include "EnemyAction.hpp"
#include "NetworkIdentity.hpp"
#include "PaintedBeamAnnouncer.hpp"
#include "PaintedBeamTimeline.hpp"
#include "PaintedBeamTimelineTestConstants.hpp"
#include "PaintedBeams.hpp"
#include "Position.hpp"

namespace rtype::client::testing {

PaintedBeamsScene::PaintedBeamsScene() : enemy_(entities_.create()) {
  components_.add(enemy_, game::NetworkIdentity{BEAMING_ENEMY_ID});
  components_.add(enemy_, AppearanceReference{&appearance_});
  components_.add(enemy_, game::Position{});
  components_.add(enemy_, PaintedBeams{});
}

void PaintedBeamsScene::announce(std::uint32_t entityId,
                                 network::EnemyActionPhase phase,
                                 std::optional<double> serverTick) {
  announcer_.followServerTick(serverTick);
  announcer_.apply(network::EnemyAction{
      .entityId = entityId, .attack = BEAM_ATTACK_NUMBER, .phase = phase});
}

void PaintedBeamsScene::drawTick(std::optional<double> renderTick) {
  showPaintedBeams(components_, renderTick);
}

const PaintedBeams& PaintedBeamsScene::beams() {
  return *components_.get<PaintedBeams>(enemy_);
}

}  // namespace rtype::client::testing
