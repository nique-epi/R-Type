#include "ReleaseFetch.hpp"
#include <httplib.h>
#include <cstddef>
#include <optional>
#include <stop_token>
#include <string>
#include <string_view>
#include "HttpStatusConstants.hpp"
#include "ReleaseConstants.hpp"

namespace rtype::client {

std::optional<std::string> fetchReleaseFile(std::string_view path,
                                            std::size_t limit,
                                            const std::stop_token& stop) {
  httplib::Client client{std::string{RELEASES_ORIGIN}};
  client.set_follow_location(true);
  client.set_connection_timeout(RELEASE_CHECK_TIMEOUT);
  client.set_read_timeout(RELEASE_CHECK_TIMEOUT);
  std::string body;
  bool tooLong = false;
  const httplib::Result result = client.Get(
      std::string{path},
      [&body, &tooLong, &stop, limit](const char* data, std::size_t length) {
        tooLong = body.size() + length > limit;
        if (!tooLong) {
          body.append(data, length);
        }
        return !tooLong && !stop.stop_requested();
      });
  if (!result || result->status != network::OK_STATUS || tooLong) {
    return std::nullopt;
  }
  return body;
}

}  // namespace rtype::client
