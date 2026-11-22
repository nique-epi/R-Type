#include "SignInText.hpp"
#include <algorithm>
#include <cctype>
#include <chrono>
#include <optional>
#include <string>
#include <string_view>
#include "DeviceLinkPhase.hpp"
#include "DeviceLinkReading.hpp"
#include "Language.hpp"
#include "SignInLines.hpp"
#include "SignInScreenConstants.hpp"
#include "SignInStanding.hpp"
#include "TextMark.hpp"
#include "TextMarks.hpp"
#include "Translation.hpp"

namespace rtype::client {

namespace {

/** @returns How long the code of @p reading stays valid at @p now, in whole
 * units of SIGN_IN_COUNT_UNIT, as the dialog writes it. */
std::string validityOf(const DeviceLinkReading& reading,
                       const SignInStanding& standing) {
  const auto units = std::chrono::floor<std::chrono::minutes>(
                         reading.expiresAt - standing.now) /
                     SIGN_IN_COUNT_UNIT;
  if (units < 1) {
    return std::string(LAST_MINUTE_TEXT.in(standing.language));
  }
  const std::string minutes = std::to_string(units);
  return withMark(std::string(VALID_FOR.in(standing.language)),
                  TextMark{.mark = MINUTES_MARK, .value = minutes});
}

SignInLines waitingLines(const DeviceLinkReading& reading,
                         const SignInStanding& standing) {
  const std::string page = pageShownOf(reading.verificationUri);
  return SignInLines{
      .title = reading.userCode,
      .line = withMark(std::string(ENTER_CODE_AT.in(standing.language)),
                       TextMark{.mark = PAGE_MARK, .value = page}),
      .validity = validityOf(reading, standing),
      .offersNewCode = false};
}

SignInLines askingLines(Language language) {
  return SignInLines{.title = std::string(SIGN_IN_TITLE.in(language)),
                     .line = std::string(ASKING_CODE_LINE.in(language)),
                     .validity = std::nullopt,
                     .offersNewCode = false};
}

SignInLines endedLines(const Translation& title, const Translation& line,
                       Language language) {
  return SignInLines{.title = std::string(title.in(language)),
                     .line = std::string(line.in(language)),
                     .validity = std::nullopt,
                     .offersNewCode = true};
}

SignInLines signedInLines(const SignInStanding& standing) {
  if (standing.sessionRefused) {
    return endedLines(SESSION_REFUSED_TITLE, SESSION_REFUSED_LINE,
                      standing.language);
  }
  return SignInLines{
      .title = std::string(SIGNED_IN_TITLE.in(standing.language)),
      .line = std::string(OPENING_ACCOUNT_LINE.in(standing.language)),
      .validity = std::nullopt,
      .offersNewCode = false};
}

}  // namespace

SignInLines signInLinesOf(const std::optional<DeviceLinkReading>& reading,
                          const SignInStanding& standing) {
  const Language language = standing.language;
  if (!reading.has_value()) {
    return askingLines(language);
  }
  switch (reading->phase) {
    case DeviceLinkPhase::Requesting:
      break;
    case DeviceLinkPhase::Waiting:
      return waitingLines(*reading, standing);
    case DeviceLinkPhase::SignedIn:
      return signedInLines(standing);
    case DeviceLinkPhase::Denied:
      return endedLines(DENIED_TITLE, DENIED_LINE, language);
    case DeviceLinkPhase::Expired:
      return endedLines(EXPIRED_TITLE, EXPIRED_LINE, language);
    case DeviceLinkPhase::Refused:
      return endedLines(REFUSED_TITLE, REFUSED_LINE, language);
    case DeviceLinkPhase::Unreachable:
      return endedLines(UNREACHABLE_TITLE, UNREACHABLE_LINE, language);
  }
  return askingLines(language);
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
