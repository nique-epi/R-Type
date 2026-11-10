#pragma once

#include <gtest/gtest.h>
#include <Velopack.hpp>
#include <array>
#include <cstdint>
#include <memory>
#include <string_view>
#include "FakeReleaseFiles.hpp"
#include "FeedConstants.hpp"
#include "SignedFeedSource.hpp"
#include "SigningKey.hpp"
#include "TemporaryFolder.hpp"

/** @brief A SignedFeedSource checking the feeds against a key made for the
 * test, over release files the test serves. */
class SignedFeedSourceFixture : public ::testing::Test {
 protected:
  SignedFeedSourceFixture();

  /** @brief Serves the feed signed by @p signer. */
  void serveFeedSignedBy(const SigningKey& signer);
  /** @returns A package of release PACKAGE_VERSION named @p fileName. */
  [[nodiscard]] static Velopack::VelopackAsset packageNamed(
      std::string_view fileName);
  /** @brief Saves @p package to the file of the test, keeping the last share
   * reported in lastProgress(). @returns What the source answered. */
  [[nodiscard]] bool save(const Velopack::VelopackAsset& package);

  [[nodiscard]] rtype::client::SignedFeedSource& source();
  [[nodiscard]] FakeReleaseFiles& files();
  [[nodiscard]] const SigningKey& key() const;
  [[nodiscard]] std::int16_t lastProgress() const;

 private:
  SigningKey key_;
  std::array<unsigned char, rtype::client::ED25519_PUBLIC_KEY_BYTES> publicKey_;
  FakeReleaseFiles* files_;
  std::unique_ptr<rtype::client::SignedFeedSource> source_;
  std::int16_t lastProgress_{};
  TemporaryFolder folder_;
};
