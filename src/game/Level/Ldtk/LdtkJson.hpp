#pragma once

#include <filesystem>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <string_view>

namespace rtype::game {

/**
 * @returns The LDtk project @p file, parsed.
 * @throws engine::UnreadableDataFileException when @p file cannot be read;
 * InvalidLdtkFileException about @p source when it is not a JSON object.
 */
[[nodiscard]] nlohmann::json ldtkProjectOf(const std::filesystem::path& file,
                                           std::string_view source);

/** @throws InvalidLdtkFileException about @p source, for @p reason followed
 * by @p found. */
[[noreturn]] void refuseLdtkFile(std::string_view source,
                                 std::string_view reason,
                                 std::string_view found);

/** @returns The member @p name of @p object.
 * @throws InvalidLdtkFileException when @p object is no object or has no
 * such member. */
[[nodiscard]] const nlohmann::json& ldtkMemberOf(const nlohmann::json& object,
                                                 std::string_view name,
                                                 std::string_view source);

/** @returns The number @p value holds, read as the member @p name.
 * @throws InvalidLdtkFileException when it holds anything else. */
[[nodiscard]] float ldtkNumberOf(const nlohmann::json& value,
                                 std::string_view name,
                                 std::string_view source);

/** @returns The text @p value holds, read as the member @p name.
 * @throws InvalidLdtkFileException when it holds anything else. */
[[nodiscard]] std::string ldtkTextOf(const nlohmann::json& value,
                                     std::string_view name,
                                     std::string_view source);

/** @returns The number the member @p name of @p object holds. */
[[nodiscard]] float ldtkNumberMemberOf(const nlohmann::json& object,
                                       std::string_view name,
                                       std::string_view source);

/** @returns The text the member @p name of @p object holds. */
[[nodiscard]] std::string ldtkTextMemberOf(const nlohmann::json& object,
                                           std::string_view name,
                                           std::string_view source);

}  // namespace rtype::game
