#pragma once

#include <string_view>

namespace rtype::network {

/** @brief The methods of the routes, as HTTP names them. */
constexpr std::string_view GET_METHOD = "GET";
constexpr std::string_view POST_METHOD = "POST";
constexpr std::string_view PUT_METHOD = "PUT";

/** @brief The media type of every body a route reads or answers. */
constexpr std::string_view JSON_CONTENT_TYPE = "application/json";

/**
 * @brief The media type of an error, a problem document of RFC 9457, whose
 * `title` is the phrase of its status.
 */
constexpr std::string_view PROBLEM_CONTENT_TYPE = "application/problem+json";

/** @brief The members of a problem document of RFC 9457 the master writes. */
constexpr std::string_view PROBLEM_TITLE_MEMBER = "title";
constexpr std::string_view PROBLEM_STATUS_MEMBER = "status";
constexpr std::string_view PROBLEM_DETAIL_MEMBER = "detail";

/** @brief The lowest status of an error, which carries a problem document. */
constexpr int LOWEST_ERROR_STATUS = 400;

/** @brief The headers a request of a game server carries. */
constexpr std::string_view CONTENT_TYPE_HEADER = "Content-Type";
constexpr std::string_view AUTHORIZATION_HEADER = "Authorization";

/**
 * @brief The scheme of RFC 6750 a key is sent with, read in any case as RFC
 * 9110 asks, and what an answer of 401 asks for.
 */
constexpr std::string_view BEARER_SCHEME = "Bearer";
constexpr std::string_view BEARER_SCHEME_SEPARATOR = " ";

}  // namespace rtype::network
