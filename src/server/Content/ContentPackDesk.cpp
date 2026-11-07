#include "ContentPackDesk.hpp"
#include <cstddef>
#include <mutex>
#include <string>
#include <utility>
#include <vector>
#include "AllowanceVerdict.hpp"
#include "ApiRequest.hpp"
#include "ApiResponse.hpp"
#include "ContentFingerprint.hpp"
#include "ContentFingerprintText.hpp"
#include "ContentPack.hpp"
#include "ContentRouteConstants.hpp"
#include "Endpoint.hpp"
#include "HttpHeader.hpp"
#include "HttpStatusConstants.hpp"
#include "IClock.hpp"
#include "MasterHttpConstants.hpp"
#include "NetworkConstants.hpp"
#include "ServerContentConstants.hpp"

namespace rtype::server {

namespace {

/** @returns The bytes of @p pack, kept as the text an answer carries. */
std::string textOf(const std::vector<std::byte>& pack) {
  std::string text(pack.size(), '\0');
  for (std::size_t index = 0; index < pack.size(); ++index) {
    text[index] = static_cast<char>(pack[index]);
  }
  return text;
}

/** @returns The fingerprint @p request names in its path, as it wrote it. */
std::string namedFingerprintOf(const network::ApiRequest& request) {
  const auto named = request.pathParameters.find(
      std::string{network::FINGERPRINT_PATH_PARAMETER});
  return named == request.pathParameters.end() ? std::string{} : named->second;
}

}  // namespace

ContentPackDesk::ContentPackDesk(std::vector<std::byte> pack,
                                 const engine::IClock& clock)
    : pack_(textOf(pack)),
      fingerprint_(network::contentFingerprintOf(pack)),
      fingerprintText_(network::fingerprintText(fingerprint_)),
      clock_(&clock),
      allowances_(PACK_REQUEST_RATE_LIMIT) {}

network::ApiResponse ContentPackDesk::answer(
    const network::ApiRequest& request) {
  if (!allows(request.remoteAddress)) {
    return network::ApiResponse{
        .status = network::TOO_MANY_REQUESTS_STATUS,
        .body = {},
        .detail = std::string{TOO_MANY_PACK_REQUESTS_DETAIL}};
  }
  if (namedFingerprintOf(request) != fingerprintText_) {
    return network::ApiResponse{.status = network::NOT_FOUND_STATUS,
                                .body = {},
                                .detail = std::string{OTHER_PACK_DETAIL}};
  }
  return network::ApiResponse{
      .status = network::OK_STATUS,
      .body = pack_,
      .header = network::HttpHeader{.name = std::string{network::ETAG_HEADER},
                                    .value = std::string{ETAG_QUOTE} +
                                             fingerprintText_ +
                                             std::string{ETAG_QUOTE}},
      .contentType = network::OCTET_STREAM_CONTENT_TYPE};
}

const network::ContentFingerprint& ContentPackDesk::fingerprint() const {
  return fingerprint_;
}

std::size_t ContentPackDesk::size() const { return pack_.size(); }

bool ContentPackDesk::allows(const std::string& address) {
  const std::scoped_lock lock{allowancesMutex_};
  return allowances_.spend(
             network::Endpoint{.address = address, .port = network::ANY_PORT},
             clock_->now()) == network::AllowanceVerdict::Allowed;
}

}  // namespace rtype::server
