#include "SpriteShowcase.hpp"
#include <optional>
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"
#include "EntitySprites.hpp"
#include "PlayfieldConstants.hpp"
#include "Position.hpp"
#include "Sprite.hpp"
#include "SpriteShowcaseConstants.hpp"

namespace rtype::client {

void spawnSpriteShowcase(engine::EntityRegistry& entities,
                         engine::ComponentRegistry& components) {
  for (const ShowcaseEntity& shown : SPRITE_SHOWCASE) {
    if (const std::optional<game::Sprite> sprite = spriteOf(shown.type);
        sprite.has_value()) {
      const engine::Entity entity = entities.create();
      components.add(entity, game::Position{.x = game::PLAYFIELD_WIDTH *
                                                 shown.shareOfPlayfieldWidth,
                                            .y = game::PLAYFIELD_HEIGHT *
                                                 SHOWCASE_ROW_SHARE_OF_HEIGHT});
      components.add(entity, *sprite);
    }
  }
}

}  // namespace rtype::client
