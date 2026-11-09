#include <gtest/gtest.h>
#include <string>
#include "ManifestSignature.hpp"
#include "ManifestSignatureTestConstants.hpp"
#include "SignedManifest.hpp"
#include "SigningKey.hpp"

using rtype::client::isSignedBy;
using rtype::client::SignedManifest;
using rtype::client::testing::CHANGED_MANIFEST;
using rtype::client::testing::SIGNED_MANIFEST;

/**
 * Given a manifest signed by a key
 * When its signature is checked against that key
 * Then it is accepted
 */
TEST(ManifestSignature, AcceptsTheSignatureOfTheKey) {
  const SigningKey key;

  EXPECT_TRUE(isSignedBy(SignedManifest{.text = SIGNED_MANIFEST,
                                        .signature = key.sign(SIGNED_MANIFEST)},
                         key.publicKey()));
}

/**
 * Given the signature of a manifest
 * When it is checked against a manifest whose version was changed after
 * Then it is refused
 */
TEST(ManifestSignature, RefusesAManifestChangedAfterItWasSigned) {
  const SigningKey key;

  EXPECT_FALSE(
      isSignedBy(SignedManifest{.text = CHANGED_MANIFEST,
                                .signature = key.sign(SIGNED_MANIFEST)},
                 key.publicKey()));
}

/**
 * Given a manifest signed by another key
 * When its signature is checked against the key of the game
 * Then it is refused
 */
TEST(ManifestSignature, RefusesTheSignatureOfAnotherKey) {
  const SigningKey key;
  const SigningKey other;

  EXPECT_FALSE(
      isSignedBy(SignedManifest{.text = SIGNED_MANIFEST,
                                .signature = other.sign(SIGNED_MANIFEST)},
                 key.publicKey()));
}

/**
 * Given the signature of a manifest, one byte short
 * When it is checked
 * Then it is refused
 */
TEST(ManifestSignature, RefusesASignatureOneByteShort) {
  const SigningKey key;
  const std::string signature = key.sign(SIGNED_MANIFEST);

  EXPECT_FALSE(isSignedBy(
      SignedManifest{.text = SIGNED_MANIFEST, .signature = signature.substr(1)},
      key.publicKey()));
}

/**
 * Given the signature of a manifest followed by one more byte
 * When it is checked
 * Then it is refused
 */
TEST(ManifestSignature, RefusesASignatureOneByteLong) {
  const SigningKey key;
  const std::string signature = key.sign(SIGNED_MANIFEST) + '\0';

  EXPECT_FALSE(isSignedBy(
      SignedManifest{.text = SIGNED_MANIFEST, .signature = signature},
      key.publicKey()));
}
