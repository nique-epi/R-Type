#include "SignInText.hpp"
#include <algorithm>
#include <cctype>
#include <chrono>
#include <optional>
#include <string>
#include <string_view>
#include "DeviceLinkPhase.hpp"
#include "DeviceLinkReading.hpp"
#include "SignInLines.hpp"
#include "SignInScreenConstants.hpp"

namespace rtype::client {

namespace {

/** @returns How long the code of @p reading stays valid at @p now, in whole
 * units of SIGN_IN_COUNT_UNIT, as the dialog writes it. */
std::string validityOf(const DeviceLinkReading& reading,
                       std::chrono::sys_seconds now) {
  const auto units =
      std::chrono::floor<std::chrono::minutes>(reading.expiresAt - now) /
      SIGN_IN_COUNT_UNIT;
  if (units < 1) {
    return std::string{LAST_MINUTE_TEXT};
  }
  return std::string{VALID_FOR_START} + std::to_string(units) +
         std::string{MINUTES_END};
}

SignInLines waitingLines(const DeviceLinkReading& reading,
                         std::chrono::sys_seconds now) {
  return SignInLines{.title = reading.userCode,
                     .line = std::string{ENTER_CODE_AT_START} +
                             pageShownOf(reading.verificationUri) +
                             validityOf(reading, now),
                     .offersNewCode = false};
}

SignInLines askingLines() {
  return SignInLines{.title = std::string{SIGN_IN_TITLE},
                     .line = std::string{ASKING_CODE_LINE},
                     .offersNewCode = false};
}

SignInLines endedLines(std::string_view title, std::string_view line) {
  return SignInLines{.title = std::string{title},
                     .line = std::string{line},
                     .offersNewCode = true};
}

SignInLines signedInLines(bool sessionRefused) {
  if (sessionRefused) {
    return endedLines(SESSION_REFUSED_TITLE, SESSION_REFUSED_LINE);
  }
  return SignInLines{.title = std::string{SIGNED_IN_TITLE},
                     .line = std::string{OPENING_ACCOUNT_LINE},
                     .offersNewCode = false};
}

}  // namespace

SignInLines signInLinesOf(const std::optional<DeviceLinkReading>& reading,
                          std::chrono::sys_seconds now, bool sessionRefused) {
  if (!reading.has_value()) {
    return askingLines();
  }
  switch (reading->phase) {
    case DeviceLinkPhase::Requesting:
      break;
    case DeviceLinkPhase::Waiting:
      return waitingLines(*reading, now);
    case DeviceLinkPhase::SignedIn:
      return signedInLines(sessionRefused);
    case DeviceLinkPhase::Denied:
      return endedLines(DENIED_TITLE, DENIED_LINE);
    case DeviceLinkPhase::Expired:
      return endedLines(EXPIRED_TITLE, EXPIRED_LINE);
    case DeviceLinkPhase::Refused:
      return endedLines(REFUSED_TITLE, REFUSED_LINE);
    case DeviceLinkPhase::Unreachable:
      return endedLines(UNREACHABLE_TITLE, UNREACHABLE_LINE);
  }
  return askingLines();
}

std::string pageShownOf(const std::string& page) {
  std::string shown = page;
  for (const std::string_view scheme : {SECURE_SCHEME, PLAIN_SCHEME}) {
    if (shown.starts_with(scheme)) {
      shown.erase(0, scheme.size());
    }
  }
  std::ranges::transform(shown, shown.begin(), [](char character) {
    return static_cast<char>(
        std::toupper(static_cast<unsigned char>(character)));
  });
  return shown;
}

}  // namespace rtype::client
