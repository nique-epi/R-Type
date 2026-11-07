#pragma once

#include <vector>
#include "EntityDestroyed.hpp"
#include "EntitySpawned.hpp"
#include "IOwnMissileListener.hpp"

/**
 * @brief An IOwnMissileListener that predicts the missiles of its player only
 * once told to, and records the messages the replicator hands it.
 */
class RecordingOwnMissileListener final
    : public rtype::client::IOwnMissileListener {
 public:
  /** @brief Makes predictsOwnMissiles() answer @p predicts. */
  void predict(bool predicts);

  [[nodiscard]] bool predictsOwnMissiles() const override;

  void ownMissileSpawned(const rtype::network::EntitySpawned& missile) override;

  void ownMissileDestroyed(
      const rtype::network::EntityDestroyed& destroyed) override;

  /** @returns The EntitySpawned handed over, oldest first. */
  [[nodiscard]] const std::vector<rtype::network::EntitySpawned>& spawned()
      const;

  /** @returns The EntityDestroyed handed over, oldest first. */
  [[nodiscard]] const std::vector<rtype::network::EntityDestroyed>& destroyed()
      const;

 private:
  bool predicts_{false};
  std::vector<rtype::network::EntitySpawned> spawned_;
  std::vector<rtype::network::EntityDestroyed> destroyed_;
};
