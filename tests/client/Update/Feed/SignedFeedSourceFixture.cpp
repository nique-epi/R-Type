#include "SignedFeedSourceFixture.hpp"
#include <Velopack.hpp>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include "FakeReleaseFiles.hpp"
#include "SignedFeedSource.hpp"
#include "SignedFeedSourceTestConstants.hpp"
#include "SigningKey.hpp"

using rtype::client::SignedFeedSource;
using rtype::client::testing::FEED_PATH;
using rtype::client::testing::FEED_SIGNATURE_PATH;
using rtype::client::testing::FEED_TEXT;
using rtype::client::testing::FULL_PACKAGE;
using rtype::client::testing::PACKAGE_ID;
using rtype::client::testing::PACKAGE_SIZE;
using rtype::client::testing::PACKAGE_VERSION;

SignedFeedSourceFixture::SignedFeedSourceFixture()
    : publicKey_(key_.publicKey()) {
  auto files = std::make_unique<FakeReleaseFiles>();
  files_ = files.get();
  source_ = std::make_unique<SignedFeedSource>(std::move(files), publicKey_);
}

void SignedFeedSourceFixture::serveFeedSignedBy(const SigningKey& signer) {
  files_->serve(std::string{FEED_PATH}, std::string{FEED_TEXT});
  files_->serve(std::string{FEED_SIGNATURE_PATH}, signer.sign(FEED_TEXT));
}

Velopack::VelopackAsset SignedFeedSourceFixture::packageNamed(
    std::string_view fileName) {
  return Velopack::VelopackAsset{.PackageId = std::string{PACKAGE_ID},
                                 .Version = std::string{PACKAGE_VERSION},
                                 .Type = std::string{FULL_PACKAGE},
                                 .FileName = std::string{fileName},
                                 .SHA1 = {},
                                 .SHA256 = {},
                                 .Size = PACKAGE_SIZE,
                                 .NotesMarkdown = {},
                                 .NotesHtml = {}};
}

bool SignedFeedSourceFixture::save(const Velopack::VelopackAsset& package) {
  return source_->DownloadReleaseEntry(
      package, (folder_.path() / package.FileName).string(),
      [this](std::int16_t progress) { lastProgress_ = progress; });
}

SignedFeedSource& SignedFeedSourceFixture::source() { return *source_; }

FakeReleaseFiles& SignedFeedSourceFixture::files() { return *files_; }

const SigningKey& SignedFeedSourceFixture::key() const { return key_; }

std::int16_t SignedFeedSourceFixture::lastProgress() const {
  return lastProgress_;
}
