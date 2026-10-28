#pragma once

#include <SFML/Graphics/Texture.hpp>
#include <functional>
#include <optional>
#include "ConnectionPhase.hpp"
#include "FlashPreference.hpp"
#include "IScreenNavigator.hpp"
#include "ISoundPlayer.hpp"
#include "MotionPreference.hpp"
#include "PlayfieldMapping.hpp"

namespace rtype::client {

/** @brief What the NO SIGNAL screen is built from, but the picture it paints
 * on. */
struct LostScreenParts {
  /** @brief Screens the screen lives among, which must outlive it. */
  std::reference_wrapper<IScreenNavigator> navigator;
  /** @brief Plays the click of the call, which must outlive the screen. */
  std::reference_wrapper<ISoundPlayer> sounds;
  /** @brief The last picture of the screens it covers, the size of the
   * playfield in pixels, which it squeezes as it turns off; or none, and it
   * turns off on the dark alone. Any texture opens the OpenGL context of
   * SFML, which a machine without a display cannot: the tests give none. */
  std::optional<sf::Texture> picture;
  /** @brief How the session ended: Lost when the server went silent, Closed
   * when it said it shut down. */
  ConnectionPhase ending{};
  /** @brief Gives the point of the playfield under a pixel of the window. */
  PlayfieldMapping playfieldPointAt;
  MotionPreference motion{};
  FlashPreference flashes{};
};

}  // namespace rtype::client
