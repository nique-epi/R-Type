#pragma once

#include <cstdint>

namespace rtype::client {

/**
 * @brief The screens the client can show.
 *
 * Every value has one case in Application::createScreen(), which builds the
 * screen; a value without a case does not compile with -Wswitch.
 */
enum class ScreenId : std::uint8_t {
  /** @brief The game in progress, entered through replaceAll() only:
   * building it releases the assets of a previous game. */
  Game,
  /** @brief The options, pushed over the game. */
  Options,
};

}  // namespace rtype::client
