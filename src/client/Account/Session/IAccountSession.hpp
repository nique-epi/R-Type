#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include "SignInRefusal.hpp"

namespace rtype::client {

/**
 * @brief The account of the master the player plays under: its token, never
 * logged, and the account the master names for it.
 *
 * signIn() asks the master who the token belongs to; the player is signed in
 * once the master answered with an account, and signed out again when the
 * master refused the token, or by signOut(). The screen that signs a player
 * in calls signIn(); at launch, the token `--session` gives is used for that
 * launch alone, and otherwise the token the previous launch kept, so the
 * player stays signed in from one launch to the next until signOut().
 */
class IAccountSession {
 public:
  virtual ~IAccountSession() = default;

  /** @returns Whether the master named the account of the newest token. */
  [[nodiscard]] virtual bool signedIn() const = 0;

  /** @returns The id of the account; NO_ACCOUNT_ID signed out. */
  [[nodiscard]] virtual std::uint64_t accountId() const = 0;

  /** @returns The nickname of the account; empty signed out. */
  [[nodiscard]] virtual std::string nickname() const = 0;

  /** @returns The token of the session; empty signed out. */
  [[nodiscard]] virtual std::string token() const = 0;

  /** @returns Why the newest token signed no one in, once the master
   * answered; nothing while it is asked, signed in or after signOut(). */
  [[nodiscard]] virtual std::optional<SignInRefusal> signInRefusal() const = 0;

  /** @brief Asks the master the account of @p token, in place of the one
   * before; the player is signed out until it answers. */
  virtual void signIn(const std::string& token) = 0;

  /** @brief Forgets the token, for this launch and the next ones: the player
   * plays as a guest. */
  virtual void signOut() = 0;
};

}  // namespace rtype::client
