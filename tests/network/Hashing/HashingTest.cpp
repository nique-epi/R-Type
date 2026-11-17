#include <gtest/gtest.h>
#include <optional>
#include "Hashing.hpp"
#include "HashingTestConstants.hpp"
#include "PasswordHash.hpp"
#include "PasswordKey.hpp"
#include "SecretComparison.hpp"

using rtype::network::PasswordHash;
using rtype::network::passwordHashOf;
using rtype::network::PasswordKey;
using rtype::network::passwordKeyOfHexadecimal;
using rtype::network::passwordMatches;
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

/**
 * Given a key written as 64 hexadecimal digits in capitals
 * When it is read
 * Then each pair of digits is one byte of the key
 */
TEST(Hashing, ReadsAKeyInAnyCase) {
  const std::optional<PasswordKey> key =
      passwordKeyOfHexadecimal(KEY_IN_CAPITALS);

  ASSERT_TRUE(key.has_value());
  EXPECT_EQ(key.value_or(PasswordKey{}).at(SECOND_BYTE), SECOND_BYTE_OF_KEY);
}

/**
 * Given a key one byte short, written as 62 digits
 * When it is read
 * Then it is no key
 */
TEST(Hashing, RefusesAKeyOneByteShort) {
  EXPECT_FALSE(passwordKeyOfHexadecimal(KEY_ONE_BYTE_SHORT).has_value());
}

/**
 * Given a key one byte too long, written as 66 digits
 * When it is read
 * Then it is no key
 */
TEST(Hashing, RefusesAKeyOneByteTooLong) {
  EXPECT_FALSE(passwordKeyOfHexadecimal(KEY_ONE_BYTE_TOO_LONG).has_value());
}

/**
 * Given 64 characters, one of which is no hexadecimal digit
 * When they are read as a key
 * Then they are no key
 */
TEST(Hashing, RefusesAKeyWithALetterPastF) {
  EXPECT_FALSE(passwordKeyOfHexadecimal(KEY_WITH_A_LETTER_PAST_F).has_value());
}

/**
 * Given the hash of a password made with a key
 * When the same password is checked with the same key
 * Then it matches
 */
TEST(Hashing, APasswordMatchesItsHash) {
  const PasswordKey key =
      passwordKeyOfHexadecimal(FIRST_KEY).value_or(PasswordKey{});

  EXPECT_TRUE(
      passwordMatches(A_PASSWORD, passwordHashOf(A_PASSWORD, key), key));
}

/**
 * Given the hash of a password made with a key
 * When another password is checked with the same key
 * Then it does not match
 */
TEST(Hashing, AnotherPasswordDoesNotMatch) {
  const PasswordKey key =
      passwordKeyOfHexadecimal(FIRST_KEY).value_or(PasswordKey{});

  EXPECT_FALSE(
      passwordMatches(ANOTHER_PASSWORD, passwordHashOf(A_PASSWORD, key), key));
}

/**
 * Given the hash of a password made with a key, as a stolen database holds it
 * When the right password is checked with another key
 * Then it does not match, so the hash alone tests no password
 */
TEST(Hashing, TheHashIsUselessWithoutItsKey) {
  const PasswordKey key =
      passwordKeyOfHexadecimal(FIRST_KEY).value_or(PasswordKey{});
  const PasswordKey otherKey =
      passwordKeyOfHexadecimal(SECOND_KEY).value_or(PasswordKey{});

  EXPECT_FALSE(
      passwordMatches(A_PASSWORD, passwordHashOf(A_PASSWORD, key), otherKey));
}

/**
 * Given one password hashed twice with one key
 * When the two hashes are compared
 * Then they differ, since each has a salt of its own
 */
TEST(Hashing, TwoHashesOfOnePasswordDiffer) {
  const PasswordKey key =
      passwordKeyOfHexadecimal(FIRST_KEY).value_or(PasswordKey{});

  const PasswordHash first = passwordHashOf(A_PASSWORD, key);
  const PasswordHash second = passwordHashOf(A_PASSWORD, key);

  EXPECT_NE(first, second);
}
