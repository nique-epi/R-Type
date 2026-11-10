#include "FakeReleaseFiles.hpp"
#include <cstddef>
#include <fstream>
#include <ios>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include "ReleaseDownload.hpp"
#include "UpdateConstants.hpp"

using rtype::client::FULL_DOWNLOAD_PERCENT;
using rtype::client::ReleaseDownload;

std::optional<std::string> FakeReleaseFiles::text(std::string_view path,
                                                  std::size_t limit) {
  const auto found = served_.find(path);
  if (found == served_.end() || found->second.size() > limit) {
    return std::nullopt;
  }
  return found->second;
}

bool FakeReleaseFiles::save(const ReleaseDownload& download) {
  const auto found = served_.find(download.path);
  if (found == served_.end()) {
    return false;
  }
  saved_.push_back(download.path);
  std::ofstream output{download.file, std::ios::binary | std::ios::trunc};
  output << found->second;
  download.progress(FULL_DOWNLOAD_PERCENT / 2);
  download.progress(FULL_DOWNLOAD_PERCENT);
  return static_cast<bool>(output.flush());
}

void FakeReleaseFiles::stop() { stopped_ = true; }

void FakeReleaseFiles::serve(const std::string& path,
                             const std::string& content) {
  served_[path] = content;
}

const std::vector<std::string>& FakeReleaseFiles::savedPaths() const {
  return saved_;
}

bool FakeReleaseFiles::stopped() const { return stopped_; }
