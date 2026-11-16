#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include "IAccountSession.hpp"
#include "SignInRefusal.hpp"

/**
 * @brief An account session a test signs in or out at will: a guest until
 * signInAs(). signIn() only records the token it was given, as a real session
 * waits for the master before it signs anyone in.
 */
class FakeAccountSession final : public rtype::client::IAccountSession {
 public:
  /** @brief Signs the player in as the account @p accountId, of @p token. */
  void signInAs(std::uint64_t accountId, const std::string& token);

  [[nodiscard]] bool signedIn() const override;
  [[nodiscard]] std::uint64_t accountId() const override;
  [[nodiscard]] std::string nickname() const override;
  [[nodiscard]] std::string token() const override;
  [[nodiscard]] std::optional<rtype::client::SignInRefusal> signInRefusal()
      const override;
  void signIn(const std::string& token) override;
  void signOut() override;

  /** @returns The token signIn() was last given; empty before. */
  [[nodiscard]] const std::string& askedToken() const;

  /** @brief The nickname of the account signed in, which nickname() gives
   * once signed in; empty by default. Public so a test sets it. */
  std::string accountNickname;

 private:
  std::optional<std::uint64_t> accountId_;
  std::string token_;
  std::string askedToken_;
};
