#pragma once

#include <optional>
#include <string>
#include "DeviceLinkReading.hpp"
#include "SignInLines.hpp"
#include "SignInStanding.hpp"

namespace rtype::client {

/**
 * @returns What the sign-in dialog shows of @p reading at the time of
 * @p standing: the code,
 * the page it is entered at and the whole minutes it stays valid, while it
 * waits; why it ended otherwise, offering a new code; and once the link
 * gave a token, that the account opens, or that it was refused, as
 * @p standing says, in its language.
 */
[[nodiscard]] SignInLines signInLinesOf(
    const std::optional<DeviceLinkReading>& reading,
    const SignInStanding& standing);

/** @returns @p page as the dialog writes it: without its scheme, in
 * capitals, `RTYPE.APP/LINK` for `https://rtype.app/link`. */
[[nodiscard]] std::string pageShownOf(const std::string& page);

}  // namespace rtype::client
