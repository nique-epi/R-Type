#pragma once

#include <cstdint>
#include <optional>
#include <vector>
#include "ContentFingerprint.hpp"
#include "EntityAppearances.hpp"
#include "IEntityTypeTable.hpp"
#include "LocalContent.hpp"
#include "NumberedEntityType.hpp"
#include "PackContent.hpp"
#include "ShipGun.hpp"

namespace rtype::client {

/**
 * @brief The entity types of the session: those of the client's own sheets
 * when the server plays the same content, or those of the pack of the server.
 *
 * The client's own content is given at construction, read from its assets,
 * with its fingerprint, so an acceptance that announces that fingerprint
 * needs no download; its appearances come once the assets module read them
 * (drawLocalWith()). A load-test bot never draws, so its own content has no
 * appearance. A session plays with one content until it closes, then
 * forget() comes back to the client's own. Not thread-safe: use it from the
 * frame loop only.
 */
class SessionEntityTypes final : public IEntityTypeTable {
 public:
  explicit SessionEntityTypes(LocalContent local);

  /** @brief Draws the client's own content with @p appearances, which must
   * outlive this table. */
  void drawLocalWith(const EntityAppearances& appearances);

  /** @returns The fingerprint of the client's own content. */
  [[nodiscard]] const network::ContentFingerprint& localFingerprint() const;

  /** @brief Plays the session with the content of the pack of the server. */
  void use(PackContent pack);

  /**
   * @brief Comes back to the client's own content: the session closed. The
   * pack stays kept until another one is used, so an animation of the last
   * frame of a game still views frames that live.
   */
  void forget();

  [[nodiscard]] const game::NumberedEntityType* entryNumbered(
      std::uint8_t number) const override;

  [[nodiscard]] const EntityAppearances& appearances() const override;

  [[nodiscard]] game::ShipGun shipGun() const override;

 private:
  [[nodiscard]] const std::vector<game::NumberedEntityType>& types() const;

  EntityAppearances none_;
  const EntityAppearances* localAppearances_{nullptr};
  LocalContent local_;
  std::optional<PackContent> pack_;
  bool playsThePack_{false};
};

}  // namespace rtype::client
