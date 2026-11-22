#pragma once

#include <functional>
#include "IAccountSession.hpp"
#include "ISoundPlayer.hpp"
#include "IWallet.hpp"
#include "Language.hpp"

namespace rtype::client {

/**
 * @brief What the account corners of the title are built from: the account
 * whose state says what they show, the wallet whose kopecks a player signed in
 * sees, the player of the click, and what each chip does.
 *
 * A std::reference_wrapper has no default constructor, so this struct is only
 * ever built with designated initializers naming every member.
 */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct TitleAccountChipsParts {
  std::reference_wrapper<const IAccountSession> account;
  std::reference_wrapper<const IWallet> wallet;
  std::reference_wrapper<ISoundPlayer> sounds;
  std::function<void()> signIn;
  std::function<void()> signOut;
  Language language = Language::English;
};

}  // namespace rtype::client
