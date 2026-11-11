#pragma once

#include <string_view>

/** @brief What FakeReleaseUpdater answers: the version installed, and the
 * reasons of its failures. */
constexpr std::string_view FAKE_INSTALLED_VERSION = "1.0.0";
constexpr std::string_view FAKE_CHECK_STEP = "check";
constexpr std::string_view FAKE_DOWNLOAD_STEP = "download";
constexpr std::string_view FAKE_APPLY_STEP = "apply";
constexpr std::string_view FAKE_FAILURE = "made to fail by the test";
