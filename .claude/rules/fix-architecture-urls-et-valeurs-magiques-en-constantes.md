---
description: Valeurs magiques — adresses, ports, nombres et chaînes porteurs de sens en constantes nommées, jamais inline
trigger: always_on
---

# RULE : Adresses, ports, magic numbers et magic strings — JAMAIS inline, toujours en constantes nommées

Pourquoi : le `.clang-tidy` du dépôt désactive `readability-magic-numbers` ; c'est cette rule qui tient la garde.

## Règle à appliquer

1. **Zéro magic number / magic string dans les corps de fonction** : tout littéral porteur de sens (tick rate, vitesse, taille de buffer, taille d'en-tête de paquet, timeout, nombre maximal de joueurs, chemin d'asset, identifiant de type de paquet) reçoit un **nom** en constante. Le test : si on doit lire le contexte pour comprendre ce que vaut le littéral, il doit être nommé.
2. **Où les ranger** : en tête du `.cpp` (namespace anonyme) si la valeur est locale au fichier ; dans un header de constantes du module (`NetworkConstants.hpp`) si elle est partagée. Une valeur partagée n'est jamais dupliquée.
3. **Adresse et port** : un défaut nommé en constante, surchargé par les arguments de la ligne de commande ; jamais une adresse écrite dans le corps d'une fonction.
4. **Cohérence intra-fichier** : si une valeur du fichier est en constante, toutes les valeurs de même nature le sont.
5. Ne sont pas magiques : `0`, `1`, `-1` dans leur sens arithmétique évident, `nullptr`, `true`/`false`.

## Exemple

- ❌ **Avant (incorrect)** :
  ```cpp
  socket_.open(asio::ip::udp::v4());
  socket_.bind({asio::ip::udp::v4(), 4242});
  std::array<std::byte, 1024> buffer{};
  if (bytesReceived < 6) {
    return;
  }
  ```
- ✅ **Après (correct)** :
  ```cpp
  // NetworkConstants.hpp
  constexpr std::uint16_t DEFAULT_SERVER_PORT = 4242;
  constexpr std::size_t MAX_DATAGRAM_SIZE = 1024;
  constexpr std::size_t PACKET_HEADER_SIZE = 6;
  ```
  ```cpp
  socket_.bind({asio::ip::udp::v4(), port_});
  std::array<std::byte, MAX_DATAGRAM_SIZE> buffer{};
  if (bytesReceived < PACKET_HEADER_SIZE) {
    return;
  }
  ```
