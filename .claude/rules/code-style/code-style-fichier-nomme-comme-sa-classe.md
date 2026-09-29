---
description: Nom de fichier = nom PascalCase exact de la classe principale qu'il déclare ou définit
paths:
  - "**/*.{cpp,hpp,tpp}"
---

# RULE : Nom de fichier = nom `PascalCase` de la classe principale

## Objectif

Le nom de fichier doit se déduire mécaniquement du nom de la classe, et inversement : on cherche `PacketReader`, on ouvre `PacketReader.hpp`. Un `packet_reader.hpp` au milieu casse la cohérence et complique la navigation.

## Règles

- Un `.hpp` / `.cpp` porte le **nom exact, en `PascalCase`, de la classe (ou de l'entité) principale** qu'il déclare/définit. Pas de `snake_case`, pas de `lowercase`, pas de `kebab-case`.
- Le préfixe `I` des interfaces fait partie du nom : `ISystem.hpp`.
- Un fichier sans classe (fonctions libres, constantes) prend le `PascalCase` du concept qu'il regroupe : `NetworkConstants.hpp`.
- Un fichier de test se nomme d'après ce qu'il teste, suffixé `Test` : `PacketReaderTest.cpp`.
- Exception : les points d'entrée `main.cpp`.
- Un fichier existant hors convention est renommé dès qu'on le touche.

## Exemples

- ❌ **Interdit** :
  ```
  packet_reader.hpp        // class PacketReader
  entityregistry.cpp       // class EntityRegistry
  udp-server.hpp           // class UdpServer
  ```
- ✅ **À la place** :
  ```
  PacketReader.hpp
  EntityRegistry.cpp
  UdpServer.hpp
  ```
