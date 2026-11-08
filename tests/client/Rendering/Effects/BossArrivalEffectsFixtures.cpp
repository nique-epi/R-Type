#include "BossArrivalEffectsFixtures.hpp"
#include <vector>
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "Position.hpp"
#include "Shockwave.hpp"

std::vector<rtype::game::Position> shockwaveCentersOf(
    rtype::engine::ComponentRegistry& components) {
  std::vector<rtype::game::Position> centers;
  components.forEach<rtype::client::Shockwave, rtype::game::Position>(
      [&centers](rtype::engine::Entity, const rtype::client::Shockwave&,
                 const rtype::game::Position& position) {
        centers.push_back(position);
      });
  return centers;
}
