#include "InputKeeper.hpp"
#include <cstdint>
#include <functional>
#include <optional>
#include <utility>
#include "ConnectedPlayer.hpp"
#include "MessageOrigin.hpp"
#include "PlayerInput.hpp"
#include "PlayerRoster.hpp"

namespace rtype::server {

namespace {

/**
 * @returns Whether @p input is newer than the input numbered @p newest: no
 * input was kept yet, or its number is higher. An input numbered like the one
 * kept is not newer: it is that input again, or one the player did not send.
 */
bool isNewer(const network::PlayerInput& input,
             const std::optional<std::uint32_t>& newest) {
  return !newest.has_value() || input.inputNumber > *newest;
}

}  // namespace

InputKeeper::InputKeeper(
    PlayerRoster& roster,
    std::function<void(const ConnectedPlayer&, const network::PlayerInput&)>
        onNewerInput)
    : roster_(roster), onNewerInput_(std::move(onNewerInput)) {}

void InputKeeper::keepIfNewer(const network::MessageOrigin& origin,
                              const network::PlayerInput& input) {
  ConnectedPlayer* const player = roster_.find(origin.sender);
  if (player == nullptr || !isNewer(input, player->newestInputNumber)) {
    return;
  }
  onNewerInput_(*player, input);
  player->newestInputNumber = input.inputNumber;
}

}  // namespace rtype::server
