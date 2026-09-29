---
description: Valider un code, une config ou un artefact contre la version que la cible exécute (CMake minimal, compilateurs de la CI, dépendances épinglées) — jamais contre la version locale qui traîne
trigger: always_on
---

# RULE : Valider contre la VERSION que la cible exécute — jamais contre la version locale qui traîne

## Règle à appliquer

1. **Établir la version que la cible exécute avant de valider** : le minimum déclaré (`cmake_minimum_required(VERSION 3.28)`, C++20), les compilateurs de la CI (GCC sur `ubuntu-latest`, MSVC sur `windows-latest`), les dépendances **épinglées** par vcpkg (baseline du `vcpkg.json` : SFML 3, Asio, GoogleTest).
2. **Pour chaque fonctionnalité « récente »** (fonction CMake, champ de preset, fonctionnalité C++20/23 de la bibliothèque standard, API SFML), vérifier sa version d'introduction et son support par les trois compilateurs avant de l'adopter. Un `std::` récent qui compile avec AppleClang peut manquer à GCC 13 ou à MSVC.
3. **Énoncer la version validée dans le rapport** : « build vert » ne veut rien dire ; « build vert sous AppleClang 17 et CMake 4.1, CI Linux/Windows à confirmer » est un résultat. Une parité non testée se dit comme un risque résiduel.
4. **Les headers de la bibliothèque utilisée sont ceux installés par vcpkg**, pas ceux d'une installation système d'une autre version (Homebrew, apt).

## Exemple

- ❌ **Avant (incorrect)** : utiliser une commande CMake apparue après 3.28 parce qu'elle marche avec le CMake local → échec sur la machine d'un coéquipier au minimum requis.
- ✅ **Après (correct)** : vérifier dans la doc CMake la version d'introduction, rester sur l'équivalent 3.28, et citer les versions testées dans la PR.
