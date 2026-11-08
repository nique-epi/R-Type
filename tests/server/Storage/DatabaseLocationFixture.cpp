#include "DatabaseLocationFixture.hpp"
#include <gtest/gtest.h>
#include <string>
#include <string_view>
#include "DatabaseLocation.hpp"
#include "DatabaseLocationTestConstants.hpp"
#include "ServerConstants.hpp"
#include "ServerStorageModuleTestConstants.hpp"
#include "TextSettingDefinition.hpp"

DatabaseLocationFixture::DatabaseLocationFixture()
    : port_(LISTENING_PORT),
      location_(registry_.declareText(rtype::engine::TextSettingDefinition{
                    .name = std::string{rtype::server::DATABASE_SETTING_NAME},
                    .defaultValue = {},
                    .help = {},
                    .validate = {}}),
                std::string{LOCATION_FOLDER}, port_) {}

void DatabaseLocationFixture::run(std::string_view line) {
  EXPECT_TRUE(interpreter_.execute(line).accepted) << line;
}

const rtype::server::DatabaseLocation& DatabaseLocationFixture::location()
    const {
  return location_;
}
