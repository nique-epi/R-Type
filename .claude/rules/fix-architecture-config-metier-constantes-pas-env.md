---
description: Réglages de gameplay = constantes typées en code ; seuls l'adresse, le port et les paramètres de déploiement viennent de la ligne de commande ou de l'environnement
trigger: always_on
---

# RULE : Le réglage de gameplay est une constante typée en code, JAMAIS un paramètre d'environnement ou de ligne de commande

## Règle à appliquer

1. **Tout réglage de gameplay ou de moteur** (vitesses, points de vie, cadence de tir, tick rate, taille de la fenêtre de prédiction, nombre maximal de joueurs par partie) est une **constante typée en code**, dans le module qui la possède. C'est une décision de conception identique pour tout le monde, qui doit passer en revue.
2. **La ligne de commande et l'environnement sont réservés au déploiement** : adresse, port, chemin des assets, niveau de log. Critère de tri : « est-ce que ça change d'une machine ou d'une exécution à l'autre ? » → oui = argument ; « est-ce une règle du jeu ? » → constante.
3. Préférer un type qui **force la complétude** : un `std::array` indexé par une `enum class` avec un `static_assert` sur sa taille, pour que l'ajout d'un type d'ennemi casse la compilation tant que ses réglages ne sont pas écrits.
4. Des données de contenu (niveaux, vagues d'ennemis) peuvent venir de **fichiers de données** versionnés ; c'est une décision explicite, pas un glissement par défaut vers un paramètre d'exécution.

## Exemple

- ❌ **Avant (incorrect)** : `./r-type_server --port 4242 --player-speed 300 --max-players 4`, lus et propagés partout.
- ✅ **Après (correct)** :
  ```cpp
  constexpr float playerSpeed = 300.0F;
  constexpr std::size_t maxPlayersPerGame = 4;
  ```
  et `./r-type_server -p 4242` : seul le port est un paramètre.
