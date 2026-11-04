#include "RefusingPlayerInputLink.hpp"
#include <cstdint>
#include <optional>
#include "ActionSet.hpp"

std::optional<std::uint32_t> RefusingPlayerInputLink::sendPlayerInput(
    [[maybe_unused]] rtype::client::ActionSet held) {
  return std::nullopt;
}
