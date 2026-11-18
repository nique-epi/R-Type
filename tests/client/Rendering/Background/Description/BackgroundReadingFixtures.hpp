#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <string>
#include <string_view>
#include <variant>
#include "BackgroundDescription.hpp"
#include "LayerDescription.hpp"
#include "ParticleContent.hpp"

/** @brief Runs a test once per background file that holds a key one of its
 * tables does not know. */
class BackgroundWithAnUnknownKey
    : public ::testing::TestWithParam<std::string_view> {};

/** @brief Runs a test once per particles, as particlesReading() takes them,
 * that hold a key they do not know. */
class ParticlesWithAnUnknownKey
    : public ::testing::TestWithParam<std::string_view> {};

/** @returns The background a file named READ_BACKGROUND_FILE holding @p text
 * describes, named READ_BACKGROUND_NAME. */
[[nodiscard]] rtype::client::BackgroundDescription backgroundReading(
    std::string_view text);

/** @returns The one layer of a background whose file holds LAYER_START, then
 * @p keys. */
[[nodiscard]] rtype::client::LayerDescription layerReading(
    std::string_view keys);

/** @returns The particles of the one layer of a background whose file holds
 * PARTICLE_LAYER_START, then @p keys. */
[[nodiscard]] rtype::client::ParticleContent particlesReading(
    std::string_view keys);

/** @returns The text of a background of @p count layers, each an image. */
[[nodiscard]] std::string layersCounting(std::size_t count);

/** @returns The content of @p layer when it is a @p Content, one built with
 * nothing otherwise. */
template <typename Content>
[[nodiscard]] Content contentOf(const rtype::client::LayerDescription& layer) {
  const auto* const content = std::get_if<Content>(&layer.content);
  return content == nullptr ? Content{} : *content;
}
