#include "InputKeeper.hpp"
#include <functional>
#include <optional>
#include <string>
#include <utility>
#include "ConnectedPlayer.hpp"
#include "Logger.hpp"
#include "MessageOrigin.hpp"
#include "PlayerInput.hpp"
#include "PlayerRoster.hpp"
#include "SessionConstants.hpp"

namespace rtype::server {

namespace {

/**
 * @returns Whether @p input is newer than @p newest: no input was kept yet, or
 * its number is higher. An input numbered like the one kept is not newer: it
 * is that input again, or one the player did not send.
 */
bool isNewer(const network::PlayerInput& input,
             const std::optional<network::PlayerInput>& newest) {
  return !newest.has_value() || input.inputNumber > newest->inputNumber;
}

/**
 * @returns Whether @p input holds other actions than @p newest, or no input
 * was kept yet.
 */
bool changesActions(const network::PlayerInput& input,
                    const std::optional<network::PlayerInput>& newest) {
  return !newest.has_value() || input.actions != newest->actions;
}

}  // namespace

InputKeeper::InputKeeper(
    PlayerRoster& roster,
    std::function<void(const ConnectedPlayer&, const network::PlayerInput&)>
        onNewerInput)
    : roster_(roster),
      onNewerInput_(std::move(onNewerInput)),
      logger_(std::string{SESSIONS_LOGGER_NAME}) {}

void InputKeeper::keepIfNewer(const network::MessageOrigin& origin,
                              const network::PlayerInput& input) {
  ConnectedPlayer* const player = roster_.find(origin.sender);
  if (player == nullptr || !isNewer(input, player->newestInput)) {
    return;
  }
  if (changesActions(input, player->newestInput)) {
    logger_.debug("player ", static_cast<unsigned int>(player->playerNumber),
                  " holds actions ", static_cast<unsigned int>(input.actions));
  }
  onNewerInput_(*player, input);
  player->newestInput = input;
}

}  // namespace rtype::server
