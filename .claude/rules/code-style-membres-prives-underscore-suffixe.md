---
description: Membres privés — underscore en suffixe (name_), jamais en préfixe ; pas d'underscore sur les membres publics
trigger: always_on
---

# RULE : Underscore en suffixe pour les membres privés

## Objectif

Le préfixe `_identifier` est réservé par le standard C++ dans certains contextes (portée globale, `_` suivi d'une majuscule) et reste ambigu avec les conventions C. Le suffixe `name_` est la convention retenue (Google C++ Style, base du `.clang-format` du dépôt). Mélanger les deux trahit du code collé d'ailleurs.

## Règles

- Les membres de données **privés et protégés** portent un underscore **en suffixe** : `socket_`, `entities_`, `tickRate_`.
- **Jamais en préfixe** : `_socket`, `m_socket`, `mSocket` sont interdits.
- Les membres **publics** n'ont pas d'underscore : les types agrégats (`struct Vector2 { float x; float y; };`, composants ECS en données pures) exposent des noms nus.
- S'applique à toute nouvelle déclaration et à toute déclaration touchée lors d'un refactoring.

## Exemples

- ❌ **Interdit** :
  ```cpp
  class UdpServer {
   private:
    asio::ip::udp::socket _socket;
    std::uint16_t m_port;
  };
  ```
- ✅ **À la place** :
  ```cpp
  class UdpServer {
   private:
    asio::ip::udp::socket socket_;
    std::uint16_t port_;
  };

  struct Velocity {
    float x;
    float y;
  };
  ```
