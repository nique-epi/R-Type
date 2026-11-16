#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include "IAccountLink.hpp"
#include "IAccountSession.hpp"
#include "SignInRefusal.hpp"
#include "UserReading.hpp"

namespace rtype::client {

/**
 * @brief The account of the player, read through an IAccountLink: every
 * question reads the newest answer of the link, kept only when it answers the
 * newest ask of signIn(), so an answer to an older token is never taken for
 * the newer one.
 */
class AccountSession final : public IAccountSession {
 public:
  /** @param link The calls to the master, which must outlive the session. */
  explicit AccountSession(IAccountLink& link);

  [[nodiscard]] bool signedIn() const override;
  [[nodiscard]] std::uint64_t accountId() const override;
  [[nodiscard]] std::string nickname() const override;
  [[nodiscard]] std::string token() const override;
  [[nodiscard]] std::optional<SignInRefusal> signInRefusal() const override;
  void signIn(const std::string& token) override;
  void signOut() override;

 private:
  /** @returns The answer of the link to the newest ask of signIn(); nothing
   * before it came, or signed out. */
  [[nodiscard]] std::optional<UserReading> answer() const;

  IAccountLink* link_;
  std::string token_;
  /** @brief The number of the newest ask of signIn(); NO_ACCOUNT_ASK after
   * signOut() or before any. */
  std::uint64_t ask_;
};

}  // namespace rtype::client
