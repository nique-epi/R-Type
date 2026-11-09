#include "ManifestSignature.hpp"
#include <openssl/evp.h>
#include <algorithm>
#include <memory>
#include <span>
#include <string_view>
#include <vector>
#include "ReleaseConstants.hpp"

namespace rtype::client {

namespace {

using PublicKey = std::unique_ptr<EVP_PKEY, decltype(&EVP_PKEY_free)>;
using VerifyContext = std::unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)>;

/** @returns The characters of @p text as the unsigned bytes OpenSSL reads. */
std::vector<unsigned char> unsignedBytesOf(std::string_view text) {
  std::vector<unsigned char> bytes(text.size());
  std::ranges::transform(text, bytes.begin(), [](char character) {
    return static_cast<unsigned char>(character);
  });
  return bytes;
}

}  // namespace

bool isSignedBy(
    std::string_view message, std::string_view signature,
    std::span<const unsigned char, ED25519_PUBLIC_KEY_BYTES> publicKey) {
  if (signature.size() != ED25519_SIGNATURE_BYTES) {
    return false;
  }
  const PublicKey key{
      EVP_PKEY_new_raw_public_key(EVP_PKEY_ED25519, nullptr, publicKey.data(),
                                  publicKey.size()),
      &EVP_PKEY_free};
  const VerifyContext context{EVP_MD_CTX_new(), &EVP_MD_CTX_free};
  if (key == nullptr || context == nullptr ||
      EVP_DigestVerifyInit(context.get(), nullptr, nullptr, nullptr,
                           key.get()) != 1) {
    return false;
  }
  const std::vector<unsigned char> signatureBytes = unsignedBytesOf(signature);
  const std::vector<unsigned char> messageBytes = unsignedBytesOf(message);
  return EVP_DigestVerify(context.get(), signatureBytes.data(),
                          signatureBytes.size(), messageBytes.data(),
                          messageBytes.size()) == 1;
}

}  // namespace rtype::client
