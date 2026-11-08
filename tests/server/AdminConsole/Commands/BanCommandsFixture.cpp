#include "BanCommandsFixture.hpp"
#include <chrono>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>
#include "BanBook.hpp"
#include "BanCommandsParts.hpp"
#include "BanCommandsTestConstants.hpp"
#include "BanTestConstants.hpp"
#include "CommandResult.hpp"

BanCommandsFixture::BanCommandsFixture()
    : now_(rtype::server::testing::BAN_MOMENT),
      commands_(rtype::server::BanCommandsParts{
          .book = book_, .output = output_, .now = [this] { return now_; }}) {
  book_.open(file(), now_);
  commands_.registerInto(interpreter_);
}

rtype::engine::CommandResult BanCommandsFixture::run(std::string_view line) {
  return interpreter_.execute(line);
}

void BanCommandsFixture::wait(std::chrono::seconds elapsed) { now_ += elapsed; }

const rtype::server::BanBook& BanCommandsFixture::book() const { return book_; }

std::filesystem::path BanCommandsFixture::file() const {
  return folder_.path() / BAN_COMMANDS_FILE_NAME;
}

std::vector<std::string> BanCommandsFixture::lines() const {
  return output_.lines();
}
