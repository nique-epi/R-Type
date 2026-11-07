#include <gtest/gtest.h>
#include <cstddef>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "ContentFile.hpp"
#include "ContentPack.hpp"
#include "ContentPackFixtures.hpp"
#include "ContentPackTestConstants.hpp"
#include "NetworkException.hpp"

using rtype::identifiers::ContentFile;
using rtype::network::contentFingerprintOf;
using rtype::network::ContentPackTooLargeException;
using rtype::network::ContentPackTrailingBytesException;
using rtype::network::ContentPackVersionException;
using rtype::network::decodeContentPack;
using rtype::network::encodeContentPack;
using rtype::network::InvalidContentPathException;
using rtype::network::isContentPath;
using rtype::network::NetworkException;
using rtype::network::TooManyContentFilesException;
using rtype::network::UnorderedContentFilesException;
using rtype::network::testing::BYDOMANCER_SPRITES_PATH;
using rtype::network::testing::DRIFTER_SHEET_PATH;
using rtype::network::testing::EMPTY_PACK_BYTES;
using rtype::network::testing::EXTRA_BYTE;
using rtype::network::testing::filesNumbered;
using rtype::network::testing::FLIPPED_BIT;
using rtype::network::testing::handWrittenPack;
using rtype::network::testing::LARGEST_PACK_BYTES;
using rtype::network::testing::LONGEST_PATH;
using rtype::network::testing::MOST_PACK_FILES;
using rtype::network::testing::ONE_BYTE;
using rtype::network::testing::ONE_DECODED_FILE;
using rtype::network::testing::ONE_FILE_PACK_OVERHEAD;
using rtype::network::testing::oneFileOfSize;
using rtype::network::testing::packAnnouncing;
using rtype::network::testing::READ_FORMAT_VERSION;
using rtype::network::testing::REFUSED_PATHS;
using rtype::network::testing::twoContentFiles;
using rtype::network::testing::UNKNOWN_FORMAT_VERSION;

/**
 * Given a sheet and a sprite sheet
 * When they are encoded as a pack, then decoded
 * Then the same files come back, path and bytes
 */
TEST(ContentPack, DecodesTheFilesItEncoded) {
  const std::vector<ContentFile> files = twoContentFiles();

  const std::vector<ContentFile> decoded =
      decodeContentPack(encodeContentPack(files));

  ASSERT_EQ(decoded.size(), files.size());
  for (std::size_t index = 0; index < files.size(); ++index) {
    EXPECT_EQ(decoded[index].path, files[index].path);
    EXPECT_EQ(decoded[index].bytes, files[index].bytes);
  }
}

/**
 * Given no file
 * When it is encoded as a pack
 * Then the pack holds its version and a count of zero only, and decodes to
 * no file
 */
TEST(ContentPack, AnEmptyPackHoldsItsHeaderOnly) {
  const std::vector<std::byte> pack = encodeContentPack({});

  EXPECT_EQ(pack.size(), EMPTY_PACK_BYTES);
  EXPECT_TRUE(decodeContentPack(pack).empty());
}

/**
 * Given as many files as a pack may hold
 * When they are encoded, then decoded
 * Then every file comes back
 */
TEST(ContentPack, HoldsTheMostFilesAPackMayHold) {
  const std::vector<std::byte> pack =
      encodeContentPack(filesNumbered(MOST_PACK_FILES));

  EXPECT_EQ(decodeContentPack(pack).size(), MOST_PACK_FILES);
}

/**
 * Given one file more than a pack may hold
 * When they are encoded
 * Then the pack is refused for its count
 */
TEST(ContentPack, RefusesOneFileTooMany) {
  EXPECT_THROW(
      static_cast<void>(encodeContentPack(filesNumbered(MOST_PACK_FILES + 1))),
      TooManyContentFilesException);
}

/**
 * Given a pack that announces one file more than a pack may hold, and holds
 * none
 * When it is decoded
 * Then it is refused for its count, before any file is read
 */
TEST(ContentPack, RefusesAnAnnouncedCountBeforeReadingAFile) {
  EXPECT_THROW(
      static_cast<void>(decodeContentPack(packAnnouncing(MOST_PACK_FILES + 1))),
      TooManyContentFilesException);
}

/**
 * Given one file whose pack takes exactly the largest size a pack may take
 * When it is encoded, then decoded
 * Then the file comes back whole
 */
TEST(ContentPack, HoldsAPackOfTheLargestSize) {
  const std::vector<std::byte> pack = encodeContentPack(
      oneFileOfSize(LARGEST_PACK_BYTES - ONE_FILE_PACK_OVERHEAD));

  EXPECT_EQ(pack.size(), LARGEST_PACK_BYTES);
  EXPECT_EQ(decodeContentPack(pack).size(), ONE_DECODED_FILE);
}

/**
 * Given one file whose pack would take one byte more than the largest size
 * When it is encoded
 * Then the pack is refused for its size
 */
TEST(ContentPack, RefusesToEncodeAPackOneByteTooLarge) {
  EXPECT_THROW(static_cast<void>(encodeContentPack(oneFileOfSize(
                   LARGEST_PACK_BYTES - ONE_FILE_PACK_OVERHEAD + 1))),
               ContentPackTooLargeException);
}

/**
 * Given bytes one more than the largest size a pack may take
 * When they are decoded
 * Then they are refused for their size, before anything is read
 */
TEST(ContentPack, RefusesToDecodeBytesOneByteTooMany) {
  const std::vector<std::byte> bytes(LARGEST_PACK_BYTES + 1);

  EXPECT_THROW(static_cast<void>(decodeContentPack(bytes)),
               ContentPackTooLargeException);
}

/**
 * Given paths that leave the folder, hold a character a path may not, are
 * empty or too long
 * When a pack names one of them, written by the encoder or by hand
 * Then the pack is refused
 */
TEST(ContentPack, RefusesAPathThatMayNotNameAFile) {
  for (const std::string_view path : REFUSED_PATHS) {
    SCOPED_TRACE(std::string{path});
    const std::vector<ContentFile> files{ContentFile{
        .path = std::string{path}, .bytes = std::string(1, ONE_BYTE)}};
    EXPECT_THROW(static_cast<void>(encodeContentPack(files)),
                 InvalidContentPathException);
    EXPECT_THROW(static_cast<void>(decodeContentPack(
                     handWrittenPack(READ_FORMAT_VERSION, path))),
                 InvalidContentPathException);
  }
}

/**
 * Given the paths of the shipped content and the longest path
 * When they are checked
 * Then each may name a file of a pack
 */
TEST(ContentPack, AcceptsTheShippedPathsAndTheLongestPath) {
  EXPECT_TRUE(isContentPath(DRIFTER_SHEET_PATH));
  EXPECT_TRUE(isContentPath(BYDOMANCER_SPRITES_PATH));
  EXPECT_TRUE(isContentPath(LONGEST_PATH));
}

/**
 * Given two files out of the order of their paths
 * When they are encoded
 * Then the pack is refused, so two servers never encode the same files in
 * two ways
 */
TEST(ContentPack, RefusesFilesOutOfOrder) {
  std::vector<ContentFile> files = twoContentFiles();
  std::swap(files[0], files[1]);

  EXPECT_THROW(static_cast<void>(encodeContentPack(files)),
               UnorderedContentFilesException);
}

/**
 * Given the same file twice
 * When they are encoded
 * Then the pack is refused
 */
TEST(ContentPack, RefusesAFileTwice) {
  std::vector<ContentFile> files = twoContentFiles();
  files[1] = files[0];

  EXPECT_THROW(static_cast<void>(encodeContentPack(files)),
               UnorderedContentFilesException);
}

/**
 * Given a pack of another version of the format
 * When it is decoded
 * Then it is refused for its version
 */
TEST(ContentPack, RefusesAnotherFormat) {
  EXPECT_THROW(static_cast<void>(decodeContentPack(handWrittenPack(
                   UNKNOWN_FORMAT_VERSION, DRIFTER_SHEET_PATH))),
               ContentPackVersionException);
}

/**
 * Given a pack followed by one more byte
 * When it is decoded
 * Then it is refused for that byte
 */
TEST(ContentPack, RefusesBytesAfterTheLastFile) {
  std::vector<std::byte> pack = encodeContentPack(twoContentFiles());
  pack.push_back(EXTRA_BYTE);

  EXPECT_THROW(static_cast<void>(decodeContentPack(pack)),
               ContentPackTrailingBytesException);
}

/**
 * Given a pack cut short at each of its sizes, from nothing to one byte short
 * When each is decoded
 * Then each is refused, never read past its end
 */
TEST(ContentPack, RefusesAPackCutShortAtEverySize) {
  const std::vector<std::byte> pack = encodeContentPack(twoContentFiles());

  for (std::size_t size = 0; size < pack.size(); ++size) {
    SCOPED_TRACE(size);
    EXPECT_THROW(
        static_cast<void>(decodeContentPack(std::span{pack.data(), size})),
        NetworkException);
  }
}

/**
 * Given a pack, and the same pack with one byte of a file changed
 * When their fingerprints are computed
 * Then the same pack gives the same fingerprint, the changed one another
 */
TEST(ContentPack, AChangedByteChangesTheFingerprint) {
  const std::vector<std::byte> pack = encodeContentPack(twoContentFiles());
  std::vector<std::byte> changed = pack;
  changed.back() = changed.back() ^ FLIPPED_BIT;

  EXPECT_EQ(contentFingerprintOf(pack), contentFingerprintOf(pack));
  EXPECT_NE(contentFingerprintOf(pack), contentFingerprintOf(changed));
}
