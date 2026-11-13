#include <gtest/gtest.h>
#include "Hashing.hpp"
#include "HashingTestConstants.hpp"
#include "SecretComparison.hpp"

using rtype::network::SecretComparison;
using rtype::network::secretsMatch;

/**
 * Given a secret a program holds
 * When a caller presents the same text
 * Then the secrets match
 */
TEST(Hashing, TheHeldSecretMatchesItself) {
  EXPECT_TRUE(secretsMatch(
      SecretComparison{.presented = HELD_SECRET, .expected = HELD_SECRET}));
}

/**
 * Given a secret a program holds
 * When a caller presents a text of the same length whose last letter differs
 * Then the secrets do not match
 */
TEST(Hashing, AGuessOfTheSameLengthDoesNotMatch) {
  EXPECT_FALSE(secretsMatch(SecretComparison{.presented = SAME_LENGTH_GUESS,
                                             .expected = HELD_SECRET}));
}

/**
 * Given a secret a program holds
 * When a caller presents only its start
 * Then the secrets do not match
 */
TEST(Hashing, TheStartOfTheSecretDoesNotMatch) {
  EXPECT_FALSE(secretsMatch(SecretComparison{.presented = START_OF_THE_SECRET,
                                             .expected = HELD_SECRET}));
}

/**
 * Given a secret a program holds
 * When a caller presents nothing
 * Then the secrets do not match
 */
TEST(Hashing, NothingDoesNotMatchASecret) {
  EXPECT_FALSE(secretsMatch(
      SecretComparison{.presented = NO_SECRET, .expected = HELD_SECRET}));
}
