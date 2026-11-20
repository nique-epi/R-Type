#include "LdtkJson.hpp"
#include <filesystem>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <string_view>
#include "DataFileReading.hpp"
#include "GameException.hpp"
#include "LdtkConstants.hpp"
#include "LdtkRefusal.hpp"

namespace rtype::game {

nlohmann::json ldtkProjectOf(const std::filesystem::path& file,
                             std::string_view source) {
  nlohmann::json project =
      nlohmann::json::parse(engine::dataFileContent(file), nullptr, false);
  if (project.is_discarded() || !project.is_object()) {
    refuseLdtkFile(source, NOT_JSON_REASON, {});
  }
  return project;
}

void refuseLdtkFile(std::string_view source, std::string_view reason,
                    std::string_view found) {
  throw InvalidLdtkFileException(
      LdtkRefusal{.file = std::string{source},
                  .reason = std::string{reason} + std::string{found}});
}

const nlohmann::json& ldtkMemberOf(const nlohmann::json& object,
                                   std::string_view name,
                                   std::string_view source) {
  if (object.is_object()) {
    const auto found = object.find(name);
    if (found != object.end()) {
      return *found;
    }
  }
  refuseLdtkFile(source, MISSING_MEMBER_REASON, name);
}

float ldtkNumberOf(const nlohmann::json& value, std::string_view name,
                   std::string_view source) {
  if (!value.is_number()) {
    refuseLdtkFile(source, NOT_A_NUMBER_REASON, name);
  }
  return value.get<float>();
}

std::string ldtkTextOf(const nlohmann::json& value, std::string_view name,
                       std::string_view source) {
  if (!value.is_string()) {
    refuseLdtkFile(source, NOT_A_TEXT_REASON, name);
  }
  return value.get<std::string>();
}

float ldtkNumberMemberOf(const nlohmann::json& object, std::string_view name,
                         std::string_view source) {
  return ldtkNumberOf(ldtkMemberOf(object, name, source), name, source);
}

std::string ldtkTextMemberOf(const nlohmann::json& object,
                             std::string_view name, std::string_view source) {
  return ldtkTextOf(ldtkMemberOf(object, name, source), name, source);
}

}  // namespace rtype::game
