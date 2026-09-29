---
description: Gestion d'erreur — exceptions custom par module dans Exceptions/, jamais de throw std:: brut ni de message en dur
trigger: always_on
---

# RULE : Les erreurs métier se lèvent via des exceptions custom du module, jamais `throw std::…` brut

## Objectif

Une exception `std::` générique levée depuis le code métier ne porte aucune information de domaine : l'appelant ne peut pas distinguer « paquet d'un type inconnu » d'un autre `std::invalid_argument` venu d'ailleurs, ni attraper *la famille* d'erreurs d'un module en un seul `catch`. Une hiérarchie custom donne un `catch` par famille, des messages centralisés et testables, et empêche une exception std de traverser une frontière de module.

## Règles

- Chaque module possède un dossier **`Exceptions/`** avec :
  - une **exception racine** du module, qui hérite de `std::runtime_error` ;
  - des **sous-classes spécifiques** par cas d'erreur.
- Il est **interdit de `throw` directement une exception de la bibliothèque standard** (`std::invalid_argument`, `std::runtime_error`, `std::out_of_range`, `std::logic_error`…).
- **Pas de message en dur dans le `throw`** : le message est construit par la classe d'exception, à partir de ses paramètres ou de constantes nommées du module.
- **Avant de créer une exception, vérifier qu'elle n'existe pas déjà** ; sinon étendre l'existante plutôt que multiplier les classes.
- Les fichiers d'`Exceptions/` sont ajoutés à la cible CMake du module.
- Une exception **ne traverse jamais la boucle réseau du serveur** : une erreur due à un client (paquet invalide, client inconnu) se traite au niveau de ce client, sans faire tomber le serveur.
- Écrire `throw std::` est le signal qu'il manque une classe dans l'`Exceptions/` du module.

## Exemples

- ❌ **Interdit** :
  ```cpp
  Room::Room(std::size_t capacity) : capacity_(capacity) {
    if (capacity == 0) {
      throw std::invalid_argument("Room capacity must be strictly positive");
    }
  }
  ```
- ✅ **À la place** :
  ```cpp
  // src/server/Lobby/Exceptions/LobbyException.hpp
  namespace rtype::lobby {

  class LobbyException : public std::runtime_error {
   public:
    explicit LobbyException(const std::string &message);
  };

  class InvalidRoomCapacityException : public LobbyException {
   public:
    explicit InvalidRoomCapacityException(std::size_t capacity);
  };

  }  // namespace rtype::lobby
  ```
  ```cpp
  // Room.cpp
  if (capacity == 0) {
    throw InvalidRoomCapacityException(capacity);
  }
  ```
