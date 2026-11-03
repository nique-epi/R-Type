#include "ConfigurationRegistryFixture.hpp"
#include <optional>
#include <string>
#include <string_view>
#include "CommandResult.hpp"
#include "ConfigurationRegistryTestConstants.hpp"
#include "EngineException.hpp"
#include "IntegerSettingDefinition.hpp"
#include "SettingValue.hpp"
#include "TextSettingDefinition.hpp"

ConfigurationRegistryFixture::ConfigurationRegistryFixture()
    : port_(&registry_.declareInteger(rtype::engine::IntegerSettingDefinition{
          .name = std::string(PORT_SETTING),
          .defaultValue = DEFAULT_PORT_VALUE,
          .lowest = LOWEST_PORT_VALUE,
          .highest = HIGHEST_PORT_VALUE,
          .help = std::string(PORT_SETTING)})),
      seed_(&registry_.declareInteger(rtype::engine::IntegerSettingDefinition{
          .name = std::string(SEED_SETTING),
          .defaultValue = std::nullopt,
          .lowest = 0,
          .highest = HIGHEST_PORT_VALUE,
          .help = std::string(SEED_SETTING),
          .allowsNone = true})),
      nickname_(&registry_.declareText(rtype::engine::TextSettingDefinition{
          .name = std::string(NICKNAME_SETTING),
          .defaultValue = std::string(DEFAULT_NICKNAME),
          .help = std::string(NICKNAME_SETTING),
          .validate = [](std::string_view value) {
            if (value == REFUSED_NICKNAME) {
              throw rtype::engine::CommandRefusedException(
                  std::string(NICKNAME_REFUSAL));
            }
          }})) {}

rtype::engine::CommandResult ConfigurationRegistryFixture::set(
    std::string_view name, std::string_view value) const {
  return interpreter_.execute("set " + std::string(name) + " " +
                              std::string(value));
}

const rtype::engine::CommandInterpreter&
ConfigurationRegistryFixture::interpreter() const {
  return interpreter_;
}

rtype::engine::ConfigurationRegistry& ConfigurationRegistryFixture::registry() {
  return registry_;
}

const rtype::engine::SettingValue& ConfigurationRegistryFixture::port() const {
  return *port_;
}

const rtype::engine::SettingValue& ConfigurationRegistryFixture::seed() const {
  return *seed_;
}

const rtype::engine::SettingValue& ConfigurationRegistryFixture::nickname()
    const {
  return *nickname_;
}
