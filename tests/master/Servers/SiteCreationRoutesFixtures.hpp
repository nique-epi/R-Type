#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "SiteClaim.hpp"

/** @returns The line of the command `set` that gives a master TEST_SITE_KEY
 * as its site key. */
[[nodiscard]] std::vector<std::string> siteKeyLines();

/**
 * @returns The status the master listening on @p port answers a creation of
 * "Paris 1" sent from the loopback with the headers of @p claim, each left
 * out when it is empty; NO_ANSWER_STATUS when the master did not answer.
 */
[[nodiscard]] int siteCreationStatus(std::uint16_t port,
                                     const rtype::master::SiteClaim& claim);

/**
 * @brief Sends the master listening on @p port every creation one address
 * may make in an hour, with the headers of @p claim, and expects each to be
 * created.
 */
void createEveryServerOfAnHour(std::uint16_t port,
                               const rtype::master::SiteClaim& claim);
