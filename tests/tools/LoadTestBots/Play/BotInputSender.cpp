#include "BotInputSender.hpp"
#include <cstdint>
#include "ActionSet.hpp"
#include "IPlayerInputLink.hpp"
#include "IWaitingRoom.hpp"
#include "TimeConstants.hpp"

namespace rtype::tools {

BotInputSender::BotInputSender(std::uint32_t seed,
                               const client::IWaitingRoom& room,
                               client::IPlayerInputLink& link)
    : room_(&room), link_(&link), pilot_(seed) {}

bool BotInputSender::advance(engine::Duration elapsed) {
  if (!room_->gameStarted()) {
    return false;
  }
  const client::ActionSet held = pilot_.advance(elapsed);
  if (!pacer_.advance(elapsed)) {
    return false;
  }
  return link_->sendPlayerInput(held).has_value();
}

}  // namespace rtype::tools
