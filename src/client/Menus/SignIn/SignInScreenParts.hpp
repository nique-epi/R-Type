#pragma once

#include <chrono>
#include <functional>
#include <string>
#include "IAccountSession.hpp"
#include "IDeviceLinkFlow.hpp"
#include "IScreenNavigator.hpp"
#include "ISoundPlayer.hpp"
#include "PlayfieldMapping.hpp"
#include "SignInReturn.hpp"

namespace rtype::client {

/** @brief What a SignInScreen is built from; every reference must outlive
 * the screen. */
struct SignInScreenParts {
  std::reference_wrapper<IScreenNavigator> navigator;
  std::reference_wrapper<IDeviceLinkFlow> deviceLinks;
  std::reference_wrapper<IAccountSession> account;
  std::reference_wrapper<SignInReturn> signInReturn;
  std::reference_wrapper<ISoundPlayer> sounds;
  PlayfieldMapping playfieldPointAt;
  /** @brief Opens a page in the browser of the player (openInBrowser()); a
   * test hands one that only records it. */
  std::function<bool(const std::string&)> openPage;
  /** @brief The clock the life of a code is counted on. */
  std::function<std::chrono::sys_seconds()> now;
};

}  // namespace rtype::client
