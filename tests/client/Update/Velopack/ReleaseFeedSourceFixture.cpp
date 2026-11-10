#include "ReleaseFeedSourceFixture.hpp"
#include <Velopack.hpp>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include "FakeReleaseFiles.hpp"
#include "ReleaseFeedSource.hpp"
#include "ReleaseFeedSourceTestConstants.hpp"

using rtype::client::ReleaseFeedSource;
using rtype::client::testing::FULL_PACKAGE;
using rtype::client::testing::PACKAGE_ID;
using rtype::client::testing::PACKAGE_SIZE;
using rtype::client::testing::PACKAGE_VERSION;

ReleaseFeedSourceFixture::ReleaseFeedSourceFixture() {
  auto files = std::make_unique<FakeReleaseFiles>();
  files_ = files.get();
  source_ = std::make_unique<ReleaseFeedSource>(std::move(files));
}

Velopack::VelopackAsset ReleaseFeedSourceFixture::packageNamed(
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

bool ReleaseFeedSourceFixture::save(const Velopack::VelopackAsset& package) {
  return source_->DownloadReleaseEntry(
      package, (folder_.path() / package.FileName).string(),
      [this](std::int16_t progress) { lastProgress_ = progress; });
}

ReleaseFeedSource& ReleaseFeedSourceFixture::source() { return *source_; }

FakeReleaseFiles& ReleaseFeedSourceFixture::files() { return *files_; }

std::int16_t ReleaseFeedSourceFixture::lastProgress() const {
  return lastProgress_;
}
