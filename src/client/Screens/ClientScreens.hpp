#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <cstddef>
#include <optional>
#include "AssetLibrary.hpp"
#include "BackgroundCatalog.hpp"
#include "GameResult.hpp"
#include "ISessionPack.hpp"
#include "ScreenContext.hpp"
#include "ScreenRegistry.hpp"
#include "ScreenServices.hpp"
#include "ScreenStack.hpp"
#include "ServerChoice.hpp"
#include "ServerSession.hpp"
#include "SessionLossWatch.hpp"
#include "Sky.hpp"
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
 * When the session with the server ends, lost or closed, the title takes the
 * place of every screen, with the dialog that says so over it; the screens of
 * the session stop there.
 *
 * Each frame, the events go to the screen on top, the sky and every screen are
 * updated, then the sky is drawn and every screen over it, from the
 * bottom up. The changes the screens ask for are applied after each event and
 * after the update, so the next event already reaches the new screen on top.
 * The sky and every screen move by the real time elapsed since the previous
 * frame, so their speed does not depend on the frame rate. The sky is drawn
 * under every screen for now, the loading screen and the options included.
 *
 * While the session plays the pack of a server that brings backgrounds, the
 * sky lays them over its own, from the frame their images are loaded, which
 * the loading screen of a game does; it takes them off the frame the session
 * is over, and never shows one whose images are not loaded.
 *
 * The screens keep pointers to it, so it can be neither copied nor moved.
 */
class ClientScreens {
 public:
  /**
   * @brief Builds the sky and the screens, then opens on openingScreen().
   * @param registry The screens the modules registered, which must outlive
   * the screens.
   * @throws RenderTextureNotCreatedException when the sky cannot get its
   * render textures.
   */
  ClientScreens(const ScreenServices& parts, const ScreenRegistry& registry);

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
  /** @brief Lays over the sky the backgrounds of the session while their
   * images are loaded, and takes them off otherwise: a session that is over,
   * or images not loaded yet or released. */
  void followSessionBackgrounds();

  const ServerSession* connection_;
  const ISessionPack* sessionPack_;
  const AssetLibrary* assets_;
  /** @brief The backgrounds of the assets, whose images are loaded for good
   * when the screens are built: the sky shows behind every screen. */
  BackgroundCatalog backgrounds_;
  Sky background_;
  GameResult gameResult_;
  std::optional<std::size_t> previousTaunt_;
  /** @brief The server, the nickname and the game every connection screen
   * starts with: those of launch, then those of the last attempt. */
  ServerChoice serverChoice_;
  /** @brief Notices, once per end, a session the server let go silent or
   * closed. */
  SessionLossWatch sessionLoss_;
  ScreenStack screens_;
  ScreenContext context_;
};

}  // namespace rtype::client
