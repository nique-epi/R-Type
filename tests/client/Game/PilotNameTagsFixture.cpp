#include "PilotNameTagsFixture.hpp"
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <utility>
#include "Caption.hpp"
#include "Entity.hpp"
#include "Layer.hpp"
#include "PilotNameTags.hpp"
#include "PilotNameTagsTestConstants.hpp"
#include "PilotShip.hpp"
#include "PilotStatus.hpp"
#include "PlayerSettings.hpp"
#include "Position.hpp"
#include "Sprite.hpp"

using rtype::client::Caption;
using rtype::client::PilotShip;
using rtype::client::PilotState;
using rtype::client::PilotStatus;
using rtype::client::PlayerSettings;
using rtype::engine::Entity;
using rtype::game::Layer;
using rtype::game::Position;
using rtype::game::Sprite;

PilotStatus pilotNamed(std::uint8_t number, std::string nickname) {
  return PilotStatus{.playerNumber = number,
                     .nickname = std::move(nickname),
                     .score = 0,
                     .lives = std::nullopt,
                     .state = PilotState::Flying,
                     .isLocalPlayer = false};
}

Entity PilotNameTagsFixture::spawnShip(int pilotColor, Position place) {
  const Entity ship = entities_.create();
  components_.add(ship, place);
  components_.add(
      ship,
      Sprite{.assetId = "ship", .frame = SHIP_FRAME, .layer = Layer::Players});
  components_.add(ship, PilotShip{.color = pilotColor});
  return ship;
}

void PilotNameTagsFixture::moveShip(Entity ship, Position place) {
  components_.add(ship, place);
}

void PilotNameTagsFixture::tag(std::span<const PilotStatus> pilots) {
  rtype::client::tagPilotShips(components_, pilots, settings_);
}

std::optional<Caption> PilotNameTagsFixture::tagOf(Entity ship) {
  const Caption* tag = components_.get<Caption>(ship);
  if (tag == nullptr) {
    return std::nullopt;
  }
  return *tag;
}

PlayerSettings& PilotNameTagsFixture::settings() { return settings_; }
