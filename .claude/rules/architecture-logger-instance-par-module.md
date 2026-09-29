---
description: Logging — une instance de Logger par module, méthodes d'instance, pas de macros ni de #ifdef NDEBUG
trigger: always_on
---

# RULE : Le Logger est une instance par module, jamais des macros ni un log conditionné par `NDEBUG`

## Objectif

Le Logger est une **facilité applicative**, pas un outil de debug : il fonctionne en release et produit les logs utiles (démarrage, connexions, erreurs, durées). Le nom du module appartient à la classe et se donne une seule fois ; le repasser à chaque appel est redondant et fragile. Des méthodes, plutôt que des macros, gagnent le typage, l'auto-complétion et la visibilité dans un débogueur.

Tant que le Logger du projet n'existe pas, cette règle fixe la forme qu'il doit avoir ; dès qu'il existe, c'est son API réelle qui fait foi.

## Règles

- Toute classe qui logge possède son propre membre `Logger logger_{"ModuleName"};` et appelle des **méthodes d'instance** (`logger_.info(...)`, `.warn`, `.error`, `.debug`).
- **Interdits** : macros de log (`LOG_INFO(...)`), API statique qui reprend le module à chaque appel (`Logger::log(level, "Module", ...)`), `std::cout` / `std::cerr` de diagnostic laissés dans le code.
- **Pas de `#ifdef NDEBUG` / `#ifndef NDEBUG`** pour décider si un log existe. Le filtrage se fait par niveau :
  - à la compilation, via une option CMake, avec `if constexpr` dans le Logger ;
  - à l'exécution, via le niveau courant du Logger (défaut `Info`, donc aucun bruit `Debug` en exécution normale).
- Une fonction libre qui logge ponctuellement peut créer un `Logger` local ; le membre d'instance reste le cas par défaut.
- Le serveur ne logge jamais une donnée par paquet au niveau `Info` : un log par datagramme sature la sortie et coûte du temps de tick.

## Exemples

- ❌ **Interdit** :
  ```cpp
  LOG_INFO("Server", "client connected " << clientId);

  #ifndef NDEBUG
  std::cerr << "[debug] tick=" << tick << '\n';
  #endif
  ```
- ✅ **À la place** :
  ```cpp
  class GameServer {
   public:
    void onClientConnected(ClientId clientId);

   private:
    Logger logger_{"GameServer"};
  };
  ```
  ```cpp
  void GameServer::onClientConnected(ClientId clientId) {
    logger_.info("client ", clientId, " connected");
  }
  ```
