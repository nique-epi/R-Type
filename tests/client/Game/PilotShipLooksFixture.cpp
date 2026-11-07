#include "PilotShipLooksFixture.hpp"
#include <optional>
#include <span>
#include "Entity.hpp"
#include "Layer.hpp"
#include "PilotShip.hpp"
#include "PilotShipLooks.hpp"
#include "PlayerLooks.hpp"
#include "Position.hpp"
#include "ShipLook.hpp"
#include "Sprite.hpp"
#include "SpriteFrame.hpp"
#include "WornLook.hpp"

using rtype::client::PilotShip;
using rtype::client::WornLook;
using rtype::engine::Entity;
using rtype::game::Layer;
using rtype::game::Position;
using rtype::game::Sprite;
using rtype::game::SpriteFrame;
using rtype::identifiers::ShipLook;

Entity PilotShipLooksFixture::spawnShip(int pilotColor) {
  const Entity ship = entities_.create();
  components_.add(ship, Position{});
  components_.add(
      ship,
      Sprite{
          .assetId = "ship",
          .frame = SpriteFrame{.left = 0, .top = 0, .width = 32, .height = 16},
          .layer = Layer::Players});
  components_.add(ship, PilotShip{.color = pilotColor});
  return ship;
}

void PilotShipLooksFixture::dress(
    std::span<const rtype::network::PlayerLook> looks) {
  rtype::client::dressPilotShips(components_, looks);
}

std::optional<ShipLook> PilotShipLooksFixture::wornBy(Entity ship) {
  const WornLook* worn = components_.get<WornLook>(ship);
  if (worn == nullptr) {
    return std::nullopt;
  }
  return worn->look;
}
