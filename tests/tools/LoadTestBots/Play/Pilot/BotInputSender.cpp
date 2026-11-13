#include "BotInputSender.hpp"
#include <cstdint>
#include <memory>
#include <utility>
#include <variant>
#include "ActionSet.hpp"
#include "BotSight.hpp"
#include "EntityMessage.hpp"
#include "IBotPilot.hpp"
#include "IPlayerInputLink.hpp"
#include "IWaitingRoom.hpp"
#include "OwnShip.hpp"
#include "ProtocolConstants.hpp"
#include "TimeConstants.hpp"
#include "WorldState.hpp"

namespace rtype::tools {

BotInputSender::BotInputSender(std::unique_ptr<IBotPilot> pilot,
                               const client::IWaitingRoom& room,
                               client::IPlayerInputLink& link)
    : room_(&room), link_(&link), pilot_(std::move(pilot)) {}

bool BotInputSender::advance(engine::Duration elapsed, const BotSight& sight) {
  if (!room_->gameStarted()) {
    return false;
  }
  const client::ActionSet held = pilot_->advance(elapsed, sight);
  if (!pacer_.advance(elapsed)) {
    return false;
  }
  return link_->sendPlayerInput(held, network::NO_SHOWN_TICK).has_value();
}

void BotInputSender::confirm(const client::EntityMessage& message,
                             std::uint8_t playerNumber) {
  const auto* state = std::get_if<network::WorldState>(&message);
  if (state == nullptr) {
    return;
  }
  if (const auto own = client::ownShipOf(*state, playerNumber)) {
    link_->confirmInputsUpTo(own->acknowledgedInput);
  }
}

}  // namespace rtype::tools
