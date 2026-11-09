#include "ReleaseCheck.hpp"
#include <mutex>
#include <optional>
#include <stop_token>
#include <string>
#include <utility>
#include "ManifestSignature.hpp"
#include "ReleaseConstants.hpp"
#include "ReleaseFetch.hpp"
#include "ReleaseManifestReading.hpp"
#include "ReleaseOffer.hpp"
#include "ReleaseVersion.hpp"
#include "SignedManifest.hpp"
#include "UpdateConstants.hpp"

namespace rtype::client {

ReleaseCheck::ReleaseCheck(std::string system, ReleaseVersion current)
    : system_(std::move(system)),
      current_(current),
      logger_(std::string{UPDATE_LOGGER_NAME}),
      thread_([this](const std::stop_token& stop) { run(stop); }) {}

bool ReleaseCheck::found() const {
  const std::scoped_lock lock{mutex_};
  return offer_.has_value();
}

std::optional<ReleaseOffer> ReleaseCheck::offer() const {
  const std::scoped_lock lock{mutex_};
  return offer_;
}

void ReleaseCheck::run(const std::stop_token& stop) {
  std::optional<ReleaseOffer> found = newerOffer(stop);
  if (!found.has_value()) {
    logger_.info("no newer release to install");
    return;
  }
  logger_.info("release ", found->versionText, " is newer: it is installed");
  const std::scoped_lock lock{mutex_};
  offer_ = std::move(found);
}

std::optional<ReleaseOffer> ReleaseCheck::newerOffer(
    const std::stop_token& stop) {
  const std::optional<std::string> manifest =
      fetchReleaseFile(LATEST_MANIFEST_PATH, MAXIMUM_MANIFEST_BYTES, stop);
  if (!manifest.has_value()) {
    logger_.info("the manifest of the latest release could not be read");
    return std::nullopt;
  }
  const std::optional<std::string> signature =
      fetchReleaseFile(LATEST_SIGNATURE_PATH, ED25519_SIGNATURE_BYTES, stop);
  if (!signature.has_value()) {
    return std::nullopt;
  }
  const SignedManifest received{.text = *manifest, .signature = *signature};
  if (!isSignedBy(received, UPDATE_PUBLIC_KEY)) {
    logger_.warn(
        "the manifest of the latest release is not signed by the key of the "
        "game: it is ignored");
    return std::nullopt;
  }
  std::optional<ReleaseOffer> offer = releaseOfferOf(received, system_);
  if (!offer.has_value() || offer->version <= current_) {
    return std::nullopt;
  }
  return offer;
}

}  // namespace rtype::client
