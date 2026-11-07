#pragma once

#include <httplib.h>
#include <cstddef>
#include <cstdint>
#include <string>

/** @returns The path of the pack of the assets next to the test executable,
 * which a supervisor started there serves. */
[[nodiscard]] std::string pathOfTheLocalPack();

/** @returns The answer of the loopback on @p port to GET @p path. */
[[nodiscard]] httplib::Result fetchedFrom(std::uint16_t port,
                                          const std::string& path);

/** @returns The answer of the loopback on @p port to GET @p path for the
 * bytes from @p offset on, as a resumed download asks. */
[[nodiscard]] httplib::Result fetchedFromOffset(std::uint16_t port,
                                                const std::string& path,
                                                std::size_t offset);
