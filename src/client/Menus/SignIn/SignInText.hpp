#pragma once

#include <chrono>
#include <optional>
#include <string>
#include "DeviceLinkReading.hpp"
#include "SignInLines.hpp"

namespace rtype::client {

/**
 * @returns What the sign-in dialog shows of @p reading at @p now: the code
 * and the page it is entered at, with the whole minutes it stays valid, while
 * it waits; why it ended otherwise, offering a new code; and once the link
 * gave a token, that the account opens, or that it was refused when
 * @p sessionRefused.
 */
[[nodiscard]] SignInLines signInLinesOf(
    const std::optional<DeviceLinkReading>& reading,
    std::chrono::sys_seconds now, bool sessionRefused);

/** @returns @p page as the dialog writes it: without its scheme, in
 * capitals, `RTYPE.APP/LINK` for `https://rtype.app/link`. */
[[nodiscard]] std::string pageShownOf(const std::string& page);

}  // namespace rtype::client
