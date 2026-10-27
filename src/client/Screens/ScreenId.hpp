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
  /** @brief The screen the client opens on, entered through replaceAll()
   * only: building it releases the assets the title does not use. */
  Title,
  /** @brief The game in progress, entered through replaceAll() only:
   * building it releases the assets of a previous game. */
  Game,
  /** @brief The options, pushed over the title, which they hide. */
  Options,
  /** @brief The options, pushed over a game, which goes on behind them,
   * dimmed. */
  GameOptions,
  /** @brief The question asked before closing the client, pushed over the
   * title. */
  QuitConfirmation,
  /**
   * @brief The server to join and the nickname to join it with, then how the
   * attempt stands, pushed over the title.
   */
  Connection,
  /**
   * @brief The players of the server joined and whether each is ready, pushed
   * over the connection screen once the server accepted the player.
   */
  Lobby,
  /** @brief The intro the client opens on, entered through replaceAll() only,
   * at launch: it replaces itself with TitleAfterIntro. */
  Intro,
  /** @brief The title screen the intro gives way to: its squadron starts in
   * place, where the ships of the intro landed. */
  TitleAfterIntro,
  /** @brief The waiting room after a game, entered through replaceAll()
   * only: alone on the stack, its BACK leaves the server for the title. */
  LobbyAfterGame,
};

}  // namespace rtype::client
