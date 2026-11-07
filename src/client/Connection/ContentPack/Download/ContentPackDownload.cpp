#include "ContentPackDownload.hpp"
#include <httplib.h>
#include <cstddef>
#include <mutex>
#include <span>
#include <stop_token>
#include <string_view>
#include <thread>
#include <utility>
#include <vector>
#include "ByteText.hpp"
#include "HttpStatusConstants.hpp"
#include "PackDownloadConstants.hpp"
#include "PackDownloadOutcome.hpp"
#include "PackDownloadProgress.hpp"
#include "PackDownloadTarget.hpp"
#include "PackTryEnd.hpp"

// NOLINTBEGIN(clang-analyzer-cplusplus.NewDeleteLeaks)
namespace rtype::client {

ContentPackDownload::ContentPackDownload(PackDownloadTarget target)
    : target_(std::move(target)),
      thread_([this](const std::stop_token& stop) { run(stop); }) {
  const std::scoped_lock lock{mutex_};
  bytes_.reserve(target_.size);
}

PackDownloadProgress ContentPackDownload::progress() const {
  const std::scoped_lock lock{mutex_};
  return PackDownloadProgress{.received = bytes_.size(), .outcome = outcome_};
}

std::vector<std::byte> ContentPackDownload::takeBytes() {
  const std::scoped_lock lock{mutex_};
  if (outcome_ != PackDownloadOutcome::Finished) {
    return {};
  }
  return std::move(bytes_);
}

void ContentPackDownload::run(const std::stop_token& stop) {
  for (std::size_t attempt = 0;
       attempt < PACK_DOWNLOAD_ATTEMPTS && !stop.stop_requested(); ++attempt) {
    const PackTryEnd end = tryOnce(stop);
    if (end == PackTryEnd::Whole) {
      finish(PackDownloadOutcome::Finished);
      return;
    }
    if (end == PackTryEnd::Refused) {
      break;
    }
  }
  finish(PackDownloadOutcome::Failed);
}

PackTryEnd ContentPackDownload::tryOnce(const std::stop_token& stop) {
  httplib::Client client{target_.host, target_.port};
  client.set_connection_timeout(PACK_CONNECT_TIMEOUT);
  client.set_read_timeout(PACK_TRANSFER_TIMEOUT);
  client.set_write_timeout(PACK_TRANSFER_TIMEOUT);
  const std::size_t from = progress().received;
  httplib::Headers headers;
  if (from > 0) {
    headers.insert(httplib::make_range_header({httplib::Range{
        static_cast<httplib::Range::first_type>(from), RANGE_TO_THE_END}}));
  }
  bool refused = false;
  const httplib::Result result = client.Get(
      target_.path, headers,
      [this, from, &refused](const httplib::Response& response) {
        refused = response.status != network::OK_STATUS &&
                  response.status != network::PARTIAL_CONTENT_STATUS;
        if (response.status == network::OK_STATUS && from > 0) {
          const std::scoped_lock lock{mutex_};
          bytes_.clear();
        }
        return !refused;
      },
      [this, &stop, &refused](const char* data, std::size_t length) {
        refused = !keep(std::span{data, length});
        return !refused && !stop.stop_requested();
      });
  if (refused) {
    return PackTryEnd::Refused;
  }
  return result && progress().received == target_.size ? PackTryEnd::Whole
                                                       : PackTryEnd::Cut;
}

bool ContentPackDownload::keep(std::span<const char> data) {
  const std::scoped_lock lock{mutex_};
  if (bytes_.size() + data.size() > target_.size) {
    return false;
  }
  const std::span<const std::byte> received =
      network::bytesOf(std::string_view{data.data(), data.size()});
  bytes_.insert(bytes_.end(), received.begin(), received.end());
  return true;
}

void ContentPackDownload::finish(PackDownloadOutcome outcome) {
  const std::scoped_lock lock{mutex_};
  outcome_ = outcome;
}

}  // namespace rtype::client
// NOLINTEND(clang-analyzer-cplusplus.NewDeleteLeaks)
