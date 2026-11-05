#include "RefusingPlayerInputLink.hpp"
#include <cstdint>
#include <optional>
#include "ActionSet.hpp"

std::optional<std::uint32_t> RefusingPlayerInputLink::sendPlayerInput(
    [[maybe_unused]] rtype::client::ActionSet held) {
  return std::nullopt;
}

void RefusingPlayerInputLink::confirmInputsUpTo(
    [[maybe_unused]] std::uint32_t acknowledged) {}
