#pragma once

#include <string_view>

/** @brief The page of the site the tests write, and what it holds. */
constexpr std::string_view WEB_INDEX_FILE = "index.html";
constexpr std::string_view WEB_INDEX_TEXT = "<h1>R-Type</h1>";

/** @brief A folder of the test the `web` setting names. */
constexpr std::string_view CHOSEN_WEB_FOLDER = "chosen-site";

/** @brief The line of configuration that turns the web module off. */
constexpr std::string_view WEB_MODULE_OFF_LINE = "module web off";

/** @brief The root of the site, as a client asks for it. */
constexpr std::string_view WEB_ROOT_URL = "/";

/** @brief The type of a problem document, which the master answers without a
 * site. */
constexpr std::string_view WEB_PROBLEM_TYPE = "application/problem+json";
