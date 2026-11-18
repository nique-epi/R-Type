#include "RefusingEntityOrders.hpp"
#include <cstdint>
#include <string>
#include <string_view>
#include "EngineException.hpp"
#include "Position.hpp"
#include "SpawnOptions.hpp"

std::uint32_t RefusingEntityOrders::spawn(
    [[maybe_unused]] std::string_view type,
    [[maybe_unused]] rtype::game::Position position,
    [[maybe_unused]] rtype::game::SpawnOptions options) {
  throw rtype::engine::ScriptException(std::string{"no body can be created"});
}

void RefusingEntityOrders::destroy([[maybe_unused]] std::uint32_t networkId) {}
