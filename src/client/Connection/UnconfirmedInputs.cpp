#include "UnconfirmedInputs.hpp"
#include <cstddef>
#include <cstdint>
#include "PlayerInput.hpp"
#include "ProtocolConstants.hpp"

namespace rtype::client {

network::PlayerInput UnconfirmedInputs::add(std::uint8_t actions) {
  const std::uint32_t number = nextNumber_++;
  kept_.push_back(actions);
  if (kept_.size() > network::MAXIMUM_REPEATED_INPUTS + 1) {
    kept_.pop_front();
  }
  network::PlayerInput message{
      .inputNumber = number, .actions = actions, .earlierActions = {}};
  message.earlierActions.reserve(kept_.size() - 1);
  for (std::size_t back = 1; back < kept_.size(); ++back) {
    message.earlierActions.push_back(kept_[kept_.size() - 1 - back]);
  }
  return message;
}

void UnconfirmedInputs::confirmUpTo(std::uint32_t acknowledged) {
  while (!kept_.empty() &&
         nextNumber_ - static_cast<std::uint32_t>(kept_.size()) <=
             acknowledged) {
    kept_.pop_front();
  }
}

void UnconfirmedInputs::clear() {
  kept_.clear();
  nextNumber_ = network::FIRST_INPUT_NUMBER;
}

}  // namespace rtype::client
