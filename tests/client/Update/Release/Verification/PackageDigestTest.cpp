#include <gtest/gtest.h>
#include <cstddef>
#include <span>
#include "ByteText.hpp"
#include "PackageDigest.hpp"
#include "PackageDigestTestConstants.hpp"

using rtype::client::sha256HexadecimalOf;
using rtype::client::testing::ABC_SHA256;
using rtype::client::testing::EMPTY_SHA256;

/**
 * Given "abc", whose SHA-256 FIPS 180-2 gives in its appendix B.1
 * When its digest is written
 * Then it is that SHA-256, in lowercase hexadecimal
 */
TEST(PackageDigest, WritesTheSha256OfTheStandard) {
  EXPECT_EQ(sha256HexadecimalOf(rtype::network::bytesOf("abc")), ABC_SHA256);
}

/**
 * Given no byte at all
 * When its digest is written
 * Then it is the SHA-256 of nothing
 */
TEST(PackageDigest, WritesTheSha256OfNothing) {
  EXPECT_EQ(sha256HexadecimalOf(std::span<const std::byte>{}), EMPTY_SHA256);
}
