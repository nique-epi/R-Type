#include <gtest/gtest.h>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <ios>
#include <optional>
#include <string>
#include <vector>
#include "ContentFile.hpp"
#include "ContentFingerprint.hpp"
#include "ContentFingerprintText.hpp"
#include "ContentPackCache.hpp"
#include "ContentPackTestFiles.hpp"
#include "ContentPackTestFilesConstants.hpp"
#include "TemporaryFolder.hpp"

using rtype::client::ContentPackCache;
using rtype::client::testing::CHANGED_BYTE;
using rtype::client::testing::fingerprintOf;
using rtype::client::testing::KEPT_PACK_EXTENSION;
using rtype::client::testing::KEPT_PACKS;
using rtype::client::testing::lancerFiles;
using rtype::client::testing::lancerPack;
using rtype::client::testing::OTHER_VALUE;
using rtype::client::testing::packNumbered;
using rtype::client::testing::TIME_BETWEEN_TWO_PACKS;
using rtype::identifiers::ContentFile;
using rtype::network::ContentFingerprint;
using rtype::network::fingerprintText;

/**
 * Given a cache that kept a pack
 * When the pack is looked for by its fingerprint
 * Then its files come back, path and bytes
 */
TEST(ContentPackCache, FindsAPackItKept) {
  const TemporaryFolder folder;
  const ContentPackCache cache{folder.path()};
  const std::vector<std::byte> pack = lancerPack();
  cache.keep(fingerprintOf(pack), pack);

  const std::optional<std::vector<ContentFile>> found =
      cache.find(fingerprintOf(pack));

  ASSERT_TRUE(found.has_value());
  const std::vector<ContentFile> files =
      found.value_or(std::vector<ContentFile>{});
  const std::vector<ContentFile> expected = lancerFiles();
  ASSERT_EQ(files.size(), expected.size());
  for (std::size_t index = 0; index < files.size(); ++index) {
    EXPECT_EQ(files[index].path, expected[index].path);
    EXPECT_EQ(files[index].bytes, expected[index].bytes);
  }
}

/**
 * Given an empty cache
 * When a pack is looked for
 * Then none is found
 */
TEST(ContentPackCache, FindsNothingItDidNotKeep) {
  const TemporaryFolder folder;
  const ContentPackCache cache{folder.path()};

  EXPECT_FALSE(cache.find(fingerprintOf(lancerPack())).has_value());
}

/**
 * Given a kept pack whose file was changed on the disk
 * When it is looked for
 * Then it is not found, and its file is gone
 */
TEST(ContentPackCache, NeverGivesAPackChangedOnTheDisk) {
  const TemporaryFolder folder;
  const ContentPackCache cache{folder.path()};
  const std::vector<std::byte> pack = lancerPack();
  const ContentFingerprint fingerprint = fingerprintOf(pack);
  cache.keep(fingerprint, pack);
  const std::filesystem::path file =
      folder.path() /
      (fingerprintText(fingerprint) + std::string{KEPT_PACK_EXTENSION});
  {
    std::fstream stream{file, std::ios::in | std::ios::out | std::ios::binary};
    stream.seekp(static_cast<std::streamoff>(CHANGED_BYTE));
    stream.put(OTHER_VALUE);
  }

  EXPECT_FALSE(cache.find(fingerprint).has_value());
  EXPECT_FALSE(std::filesystem::exists(file));
}

/**
 * Given a cache that kept as many packs as it keeps, the first one read again
 * since
 * When it keeps one more
 * Then the pack used longest ago is gone, and the one read again stays
 */
TEST(ContentPackCache, ForgetsThePackUsedLongestAgo) {
  const TemporaryFolder folder;
  const ContentPackCache cache{folder.path()};
  std::vector<ContentFingerprint> kept;
  for (std::size_t seed = 0; seed < KEPT_PACKS; ++seed) {
    const std::vector<std::byte> pack = packNumbered(seed);
    kept.push_back(fingerprintOf(pack));
    cache.keep(kept.back(), pack);
    const std::filesystem::path file =
        folder.path() /
        (fingerprintText(kept.back()) + std::string{KEPT_PACK_EXTENSION});
    std::filesystem::last_write_time(
        file, std::filesystem::file_time_type::clock::now() -
                  (TIME_BETWEEN_TWO_PACKS * (KEPT_PACKS - seed)));
  }
  ASSERT_TRUE(cache.find(kept.front()).has_value());

  const std::vector<std::byte> newest = packNumbered(KEPT_PACKS);
  cache.keep(fingerprintOf(newest), newest);

  EXPECT_TRUE(cache.find(kept.front()).has_value());
  EXPECT_FALSE(cache.find(kept[1]).has_value());
  EXPECT_TRUE(cache.find(fingerprintOf(newest)).has_value());
}
