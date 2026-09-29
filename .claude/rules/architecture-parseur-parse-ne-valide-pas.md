---
description: Un parseur/désérialiseur transforme, il ne valide pas — la validation des entrées non fiables vit dans une couche distincte en amont
trigger: always_on
---

# RULE : Un parseur parse, il ne valide pas — la validation vit dans une couche dédiée en amont

## Objectif

Séparer les responsabilités. Mêler transformation et validation gonfle le parseur, multiplie des chemins d'erreur pénibles à tester et disperse la garantie de robustesse. Un parseur pur (entrée → structure, sans branche d'erreur) est trivial à lire, à tester et à réutiliser ; une couche de validation unique est le seul endroit à auditer pour savoir ce que le serveur accepte.

Sur R-Type, les datagrammes viennent de clients **non fiables** : la validation n'est pas optionnelle, elle est **localisée**.

## Règles

- Une fonction ou un module de **parsing / désérialisation** (paquets réseau, fichiers de niveau, configuration) fait **une seule chose** : transformer une entrée **déjà validée** en structure de données. Il ne vérifie pas le format, ne lève pas d'exception de format, n'embarque pas de `try/catch` de garde.
- **Toute entrée non fiable passe d'abord par une couche de validation distincte**, placée **avant** le parseur : taille minimale et maximale, type de paquet connu, longueurs annoncées cohérentes avec les octets reçus, bornes des index et identifiants.
- La couche de validation **rejette** une entrée invalide (paquet ignoré, client signalé) ; elle ne la « répare » pas.
- Le parseur peut donc **supposer** ses préconditions ; elles sont écrites dans son doc-comment.
- Cette règle précise `architecture-exceptions-custom-par-module.md` : celle-là dit *comment* lever une erreur métier, celle-ci dit que le parseur n'en lève pas.
- Quand on est tenté d'ajouter une vérification dans un parseur, c'est le signal qu'elle manque dans la couche de validation.

## Exemples

- ❌ **Interdit** :
  ```cpp
  PlayerInput PacketParser::parseInput(std::span<const std::byte> payload) {
    if (payload.size() < inputPayloadSize) {
      throw MalformedPacketException(payload.size());
    }
    try {
      return PlayerInput{readUint32(payload, 0), readUint8(payload, 4)};
    } catch (...) {
      return PlayerInput{};
    }
  }
  ```
- ✅ **À la place** :
  ```cpp
  // PacketValidator: the only place that decides what the server accepts.
  bool PacketValidator::isValidInput(std::span<const std::byte> payload) const;

  // PacketParser: assumes a validated payload.
  PlayerInput PacketParser::parseInput(std::span<const std::byte> payload) {
    return PlayerInput{readUint32(payload, 0), readUint8(payload, 4)};
  }
  ```
  ```cpp
  if (!validator_.isValidInput(payload)) {
    logger_.warn("dropped malformed input packet from client ", clientId);
    return;
  }
  handleInput(clientId, parser_.parseInput(payload));
  ```
