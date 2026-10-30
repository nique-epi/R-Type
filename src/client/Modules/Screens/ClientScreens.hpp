#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Window/Event.hpp>
#include <cstddef>
#include <optional>
#include "ClientScreensParts.hpp"
#include "GameResult.hpp"
#include "ScreenCatalog.hpp"
#include "ScreenStack.hpp"
#include "ScrollingBackground.hpp"
#include "ServerChoice.hpp"
#include "ServerConnection.hpp"
#include "SessionLossWatch.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The screens of the client and what they share: the sky drawn behind
 * every screen and the result of the last game. The sounds and the music come
 * from the audio service of the sound module: the theme is started by the
 * intro after its black second or by a title screen, and the music of a game
 * by a game, a silence after stopping the theme. When the server ends a game,
 * the game screen writes its result here and gives way to the end screen, from
 * which the player plays again in the waiting room or quits to the title.
 * When the session with the server ends, lost or closed, the NO SIGNAL card
 * takes the place of every screen, on the last picture they showed; the
 * screens of the session stop there.
 *
 * Each frame, the events go to the screen on top, the sky and every screen are
 * updated, then the sky is drawn and every screen over it, from the
 * bottom up. The changes the screens ask for are applied after each event and
 * after the update, so the next event already reaches the new screen on top.
 * The sky and every screen move by the real time elapsed since the previous
 * frame, so their speed does not depend on the frame rate. The sky is drawn
 * under every screen for now, the loading screen and the options included.
 *
 * The screens keep pointers to it, so it can be neither copied nor moved.
 */
class ClientScreens {
 public:
  /**
   * @brief Builds the sky and the screens, then opens
   * on ScreenCatalog::firstScreen().
   * @throws RenderTextureNotCreatedException when the sky cannot get its
   * render textures.
   */
  explicit ClientScreens(const ClientScreensParts& parts);

  ClientScreens(const ClientScreens&) = delete;
  ClientScreens& operator=(const ClientScreens&) = delete;
  ClientScreens(ClientScreens&&) = delete;
  ClientScreens& operator=(ClientScreens&&) = delete;
  ~ClientScreens() = default;

  /** @brief Gives @p event to the screen on top, then applies the changes it
   * asked for. */
  void handleEvent(const sf::Event& event);

  /**
   * @brief Updates the sky and every screen by @p elapsed, then
   * applies the changes the screens asked for.
   * @throws AssetLoadingException when an asset of the game cannot be loaded.
   * @throws RenderTextureNotCreatedException when the last picture of the
   * screens of a session that ended cannot be made.
   */
  void update(engine::Duration elapsed);

  /**
   * @brief Draws the sky, then the screens over it, from the bottom up.
   * @throws RenderTextureNotCreatedException when the sky cannot be enlarged
   * to the size of the window.
   */
  void draw(sf::RenderTarget& target);

  [[nodiscard]] bool isEmpty() const;

 private:
  const ServerConnection* connection_;
  ScrollingBackground background_;
  GameResult gameResult_;
  std::optional<std::size_t> previousTaunt_;
  /** @brief The server and the nickname every connection screen starts
   * with: those of launch, then those of the last attempt. */
  ServerChoice serverChoice_;
  /** @brief Notices, once per end, a session the server let go silent or
   * closed. */
  SessionLossWatch sessionLoss_;
  /** @brief The last picture of the screens of a session that ended, taken
   * before the NO SIGNAL card replaces them, which it moves into it. */
  sf::Texture lostPicture_;
  ScreenStack screens_;
  ScreenCatalog catalog_;
};

}  // namespace rtype::client
