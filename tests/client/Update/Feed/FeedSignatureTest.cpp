#include <gtest/gtest.h>
#include <string>
#include "FeedSignature.hpp"
#include "FeedSignatureTestConstants.hpp"
#include "SignedFeed.hpp"
#include "SigningKey.hpp"

using rtype::client::isSignedBy;
using rtype::client::SignedFeed;
using rtype::client::testing::CHANGED_FEED;
using rtype::client::testing::SIGNED_FEED;

/**
 * Given a feed signed by a key
 * When its signature is checked against that key
 * Then it is accepted
 */
TEST(FeedSignature, AcceptsTheSignatureOfTheKey) {
  const SigningKey key;

  EXPECT_TRUE(isSignedBy(
      SignedFeed{.text = SIGNED_FEED, .signature = key.sign(SIGNED_FEED)},
      key.publicKey()));
}

/**
 * Given the signature of a feed
 * When it is checked against a feed whose version was changed after
 * Then it is refused
 */
TEST(FeedSignature, RefusesAFeedChangedAfterItWasSigned) {
  const SigningKey key;

  EXPECT_FALSE(isSignedBy(
      SignedFeed{.text = CHANGED_FEED, .signature = key.sign(SIGNED_FEED)},
      key.publicKey()));
}

/**
 * Given a feed signed by another key
 * When its signature is checked against the key of the game
 * Then it is refused
 */
TEST(FeedSignature, RefusesTheSignatureOfAnotherKey) {
  const SigningKey key;
  const SigningKey other;

  EXPECT_FALSE(isSignedBy(
      SignedFeed{.text = SIGNED_FEED, .signature = other.sign(SIGNED_FEED)},
      key.publicKey()));
}

/**
 * Given the signature of a feed, one byte short
 * When it is checked
 * Then it is refused
 */
TEST(FeedSignature, RefusesASignatureOneByteShort) {
  const SigningKey key;
  const std::string signature = key.sign(SIGNED_FEED);

  EXPECT_FALSE(isSignedBy(
      SignedFeed{.text = SIGNED_FEED, .signature = signature.substr(1)},
      key.publicKey()));
}

/**
 * Given the signature of a feed followed by one more byte
 * When it is checked
 * Then it is refused
 */
TEST(FeedSignature, RefusesASignatureOneByteLong) {
  const SigningKey key;
  const std::string signature = key.sign(SIGNED_FEED) + '\0';

  EXPECT_FALSE(
      isSignedBy(SignedFeed{.text = SIGNED_FEED, .signature = signature},
                 key.publicKey()));
}
