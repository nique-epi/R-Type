#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include "IAccountLink.hpp"
#include "IAccountSession.hpp"
#include "ISessionTokenStorage.hpp"
#include "SignInRefusal.hpp"
#include "UserReading.hpp"

namespace rtype::client {

/**
 * @brief The account of the player, read through an IAccountLink: every
 * question reads the newest answer of the link, kept only when it answers the
 * newest ask of signIn(), so an answer to an older token is never taken for
 * the newer one.
 *
 * The token the master named an account for is kept in an
 * ISessionTokenStorage, so the next launch signs in with it through
 * restore(). It is forgotten by signOut(), and when the master refuses it
 * (SignInRefusal::TokenRefused: unknown or expired); a master that cannot
 * answer (SignInRefusal::MasterRefused) leaves it kept, for the next try.
 */
class AccountSession final : public IAccountSession {
 public:
  /**
   * @param link The calls to the master, which must outlive the session.
   * @param storage Where the token is kept between launches, which must
   * outlive the session.
   */
  AccountSession(IAccountLink& link, ISessionTokenStorage& storage);

  [[nodiscard]] bool signedIn() const override;
  [[nodiscard]] std::uint64_t accountId() const override;
  [[nodiscard]] std::string nickname() const override;
  [[nodiscard]] std::string token() const override;
  [[nodiscard]] std::optional<SignInRefusal> signInRefusal() const override;
  void signIn(const std::string& token) override;
  void signOut() override;

  /** @brief Signs in with the token the previous launch kept, if any. */
  void restore();

  /**
   * @brief Keeps the token once the master named its account, and forgets
   * the kept one once the master refused the newest token. Called every
   * frame: it reads the link only until the newest ask is answered, and
   * writes the storage only when what it holds changes.
   */
  void rememberAnswer();

 private:
  /** @returns The answer of the link to the newest ask of signIn(); nothing
   * before it came, or signed out. */
  [[nodiscard]] std::optional<UserReading> answer() const;

  IAccountLink* link_;
  ISessionTokenStorage* storage_;
  std::string token_;
  /** @brief The token the storage holds; empty when it holds none. */
  std::string rememberedToken_;
  /** @brief The number of the newest ask of signIn(); NO_ACCOUNT_ASK after
   * signOut() or before any. */
  std::uint64_t ask_;
  /** @brief The newest ask whose answer rememberAnswer() handled;
   * NO_ACCOUNT_ASK before any. */
  std::uint64_t rememberedAsk_;
};

}  // namespace rtype::client
