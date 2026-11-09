#include "SigningKey.hpp"
#include <openssl/evp.h>
#include <array>
#include <cstddef>
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include "ReleaseConstants.hpp"

namespace {

EVP_PKEY* keyOf(void* key) { return static_cast<EVP_PKEY*>(key); }

}  // namespace

SigningKey::SigningKey()
    : key_(EVP_PKEY_Q_keygen(nullptr, nullptr, "ED25519")) {}

SigningKey::~SigningKey() { EVP_PKEY_free(keyOf(key_)); }

std::string SigningKey::sign(std::string_view message) const {
  const std::unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)> context{
      EVP_MD_CTX_new(), &EVP_MD_CTX_free};
  const std::vector<unsigned char> text(message.begin(), message.end());
  std::array<unsigned char, rtype::client::ED25519_SIGNATURE_BYTES> signature{};
  std::size_t length = signature.size();
  EVP_DigestSignInit(context.get(), nullptr, nullptr, nullptr, keyOf(key_));
  EVP_DigestSign(context.get(), signature.data(), &length, text.data(),
                 text.size());
  return {signature.begin(), signature.end()};
}

std::array<unsigned char, rtype::client::ED25519_PUBLIC_KEY_BYTES>
SigningKey::publicKey() const {
  std::array<unsigned char, rtype::client::ED25519_PUBLIC_KEY_BYTES> key{};
  std::size_t length = key.size();
  EVP_PKEY_get_raw_public_key(keyOf(key_), key.data(), &length);
  return key;
}
