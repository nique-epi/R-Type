#include <gtest/gtest.h>
#include <algorithm>
#include <array>
#include "ServerToClientDictionaryConstants.hpp"
#include "ZstdDictionaryTestConstants.hpp"

using rtype::network::SERVER_TO_CLIENT_DICTIONARY;
using rtype::network::testing::DICTIONARY_MAGIC_NUMBER;

/**
 * Given the dictionary the protocol ships
 * When its first four bytes are read
 * Then they are the magic number of a zstd dictionary, so a tool that wrote
 *      something else is noticed
 */
TEST(ZstdDictionary, StartsWithTheMagicNumberOfAZstdDictionary) {
  EXPECT_TRUE(std::equal(DICTIONARY_MAGIC_NUMBER.begin(),
                         DICTIONARY_MAGIC_NUMBER.end(),
                         SERVER_TO_CLIENT_DICTIONARY.begin()));
}
