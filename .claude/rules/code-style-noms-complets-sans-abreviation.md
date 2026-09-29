---
description: Identifiants — noms complets et explicites, jamais d'abréviation, même idiomatique dans le domaine
trigger: always_on
---

# RULE : Pas d'abréviation dans les identifiants

## Objectif

Une abréviation idiomatique pour qui connaît le domaine est opaque pour tout autre lecteur — un coéquipier qui découvre le module réseau, le jury Epitech. Le code est lu bien plus souvent qu'écrit : le coût à l'écriture est négligeable, le gain en lisibilité est durable.

## Règles

- Méthodes, fonctions, classes, membres et variables utilisent des **noms complets et explicites**, jamais des abréviations — même quand l'abréviation est répandue dans le domaine (`pos`, `vel`, `dt`, `ent`, `cmp`, `sys`, `pkt`, `buf`, `cfg`, `nb`, `idx`, `mgr`).
- Seules exceptions :
  - les noms de la bibliothèque standard et des dépendances (`std::size_t`, `asio::ip::udp`) ;
  - les acronymes devenus des mots (`id`, `udp`, `tcp`, `ecs`, `ui`), écrits comme des mots : `clientId`, `udpSocket`.
  - les compteurs de boucle triviaux sur un index (`i`) quand la boucle tient en quelques lignes.
- S'applique à toute nouvelle déclaration et à toute déclaration touchée lors d'un refactoring.

## Exemples

- ❌ **Interdit** :
  ```cpp
  void updatePos(Entity ent, float dt);
  std::size_t nbPkts;
  PacketMgr pktMgr;
  ```
- ✅ **À la place** :
  ```cpp
  void updatePosition(Entity entity, float deltaTime);
  std::size_t packetCount;
  PacketDispatcher packetDispatcher;
  ```
