#include "LdtkPlacementFixtures.hpp"
#include <string>
#include <string_view>
#include "LdtkPlacement.hpp"
#include "Position.hpp"

rtype::game::LdtkPlacement placedEnemyOf(std::string_view sheet,
                                         rtype::game::Position center,
                                         float width) {
  return rtype::game::LdtkPlacement{.entity = std::string{sheet},
                                    .sheet = std::string{sheet},
                                    .position = center,
                                    .width = width,
                                    .fields = {}};
}

std::string oneLevelProject(std::string_view entityInstances) {
  return std::string{R"json({"levels": [{
    "identifier": "Level_1", "pxWid": 480, "pxHei": 270,
    "layerInstances": [
      {"__identifier": "Walls", "__type": "Tiles"},
      {"__identifier": "Mobs", "__type": "Entities", "entityInstances": [)json"} +
         std::string{entityInstances} + std::string{R"json(]}
    ]
  }]})json"};
}
