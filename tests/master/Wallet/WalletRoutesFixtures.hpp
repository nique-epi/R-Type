#pragma once

#include <httplib.h>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include "RouteCall.hpp"

/** @returns A call over HTTPS of @p method on @p path with @p token as Bearer,
 * none when it is empty, and no body. */
[[nodiscard]] RouteCall ownCall(std::string_view method, std::string_view path,
                                std::string_view token);

/** @returns A `POST` of @p body to the cashier, over HTTPS, with @p token as
 * Bearer. */
[[nodiscard]] RouteCall exchangeCall(std::string_view token, std::string body);

/** @returns The whole number of the member @p member of the JSON object
 * @p answer holds; nothing when it holds none. */
[[nodiscard]] std::optional<std::int64_t> numberIn(
    const httplib::Result& answer, std::string_view member);

/** @returns How many elements the array of the member @p member of the JSON
 * object @p answer holds; nothing when it holds no such array. */
[[nodiscard]] std::optional<std::size_t> arraySizeIn(
    const httplib::Result& answer, std::string_view member);
