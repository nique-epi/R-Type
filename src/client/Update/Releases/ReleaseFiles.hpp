#pragma once

#include <atomic>
#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include "IReleaseFiles.hpp"
#include "ReleaseDownload.hpp"

namespace rtype::client {

/** @brief The files of the releases, read over HTTPS from RELEASES_ORIGIN with
 * cpp-httplib, following its redirects to the storage of GitHub; each request
 * waits at most RELEASE_REQUEST_TIMEOUT to connect and for each read. */
class ReleaseFiles final : public IReleaseFiles {
 public:
  [[nodiscard]] std::optional<std::string> text(std::string_view path,
                                                std::size_t limit) override;
  [[nodiscard]] bool save(const ReleaseDownload& download) override;
  void stop() override;

 private:
  std::atomic<bool> stopped_{false};
};

}  // namespace rtype::client
