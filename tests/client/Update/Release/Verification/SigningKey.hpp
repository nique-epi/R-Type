#pragma once

#include <array>
#include <string>
#include <string_view>
#include "ReleaseConstants.hpp"

/** @brief An Ed25519 key made for the test, as `openssl genpkey -algorithm
 * ed25519` makes the key of the releases. */
class SigningKey {
 public:
  SigningKey();
  ~SigningKey();
  SigningKey(const SigningKey&) = delete;
  SigningKey& operator=(const SigningKey&) = delete;
  SigningKey(SigningKey&&) = delete;
  SigningKey& operator=(SigningKey&&) = delete;

  /** @returns The signature of @p message, as `openssl pkeyutl -sign -rawin`
   * writes it. */
  [[nodiscard]] std::string sign(std::string_view message) const;
  [[nodiscard]] std::array<unsigned char,
                           rtype::client::ED25519_PUBLIC_KEY_BYTES>
  publicKey() const;

 private:
  void* key_;
};
