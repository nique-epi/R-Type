#include "ReleaseFiles.hpp"
#include <httplib.h>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <ios>
#include <optional>
#include <string>
#include <string_view>
#include "HttpStatusConstants.hpp"
#include "ReleaseDownload.hpp"
#include "ReleasesConstants.hpp"
#include "UpdateConstants.hpp"

// NOLINTBEGIN(clang-analyzer-cplusplus.NewDeleteLeaks)
namespace rtype::client {

namespace {

/** @returns A client of RELEASES_ORIGIN that follows redirects and waits at
 * most RELEASE_REQUEST_TIMEOUT. */
httplib::Client originClient() {
  httplib::Client client{std::string{RELEASES_ORIGIN}};
  client.set_follow_location(true);
  client.set_connection_timeout(RELEASE_REQUEST_TIMEOUT);
  client.set_read_timeout(RELEASE_REQUEST_TIMEOUT);
  return client;
}

/** @returns The share of @p size that @p received is, from 0 to
 * FULL_DOWNLOAD_PERCENT. */
int percentOf(std::uint64_t received, std::uint64_t size) {
  if (size == 0) {
    return FULL_DOWNLOAD_PERCENT;
  }
  return static_cast<int>(received * FULL_DOWNLOAD_PERCENT / size);
}

}  // namespace

std::optional<std::string> ReleaseFiles::text(std::string_view path,
                                              std::size_t limit) {
  httplib::Client client = originClient();
  std::string body;
  bool tooLong = false;
  const httplib::Result result = client.Get(
      std::string{path},
      [this, &body, &tooLong, limit](const char* data, std::size_t length) {
        tooLong = body.size() + length > limit;
        if (!tooLong) {
          body.append(data, length);
        }
        return !tooLong && !stopped_;
      });
  if (!result || result->status != network::OK_STATUS || tooLong) {
    return std::nullopt;
  }
  return body;
}

bool ReleaseFiles::save(const ReleaseDownload& download) {
  httplib::Client client = originClient();
  std::ofstream output{download.file, std::ios::binary | std::ios::trunc};
  std::uint64_t received = 0;
  const httplib::Result result =
      client.Get(download.path, [this, &output, &received, &download](
                                    const char* data, std::size_t length) {
        received += length;
        if (received > download.size || stopped_) {
          return false;
        }
        output.write(data, static_cast<std::streamsize>(length));
        download.progress(percentOf(received, download.size));
        return static_cast<bool>(output);
      });
  return result && result->status == network::OK_STATUS &&
         received == download.size && static_cast<bool>(output.flush());
}

void ReleaseFiles::stop() { stopped_ = true; }

}  // namespace rtype::client
// NOLINTEND(clang-analyzer-cplusplus.NewDeleteLeaks)
