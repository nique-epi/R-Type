#pragma once

#include <gtest/gtest.h>
#include <string_view>

/** @brief Runs a test once per asset id the rule accepts. */
class AcceptedAssetId : public ::testing::TestWithParam<std::string_view> {};

/** @brief Runs a test once per asset id the rule rejects. */
class RejectedAssetId : public ::testing::TestWithParam<std::string_view> {};
