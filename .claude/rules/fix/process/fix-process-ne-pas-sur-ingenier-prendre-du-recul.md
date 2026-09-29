---
description: Ne pas sur-ingénier — chercher la bonne façon de faire, stop au deuxième aller-retour, prendre du recul sur tout le flow
trigger: always_on
---

# RULE : Ne pas sur-ingénier — chercher la bonne façon de faire, puis prendre du recul sur tout le flow

## Règle à appliquer

**Avant** d'écrire la moindre ligne sur un sujet mal maîtrisé — et **impérativement** au deuxième retour de l'utilisateur sur la même chose :

1. **STOP au 2ᵉ aller-retour.** Deux corrections sur le même point = le problème n'est pas là où je le cherche. On arrête de patcher.
2. **Chercher la bonne façon de faire, ne jamais deviner** : la doc de la bibliothèque (SFML, Asio, GoogleTest, CMake), ses headers installés par vcpkg, une recherche web si besoin. Une hypothèse technique non vérifiée ne se code pas ; elle se vérifie, ou se dit comme une hypothèse.
3. **Prendre du recul sur tout le flow**, pas sur la ligne fautive : dérouler la fonctionnalité comme un diagramme de séquence (qui appelle quoi, dans quel ordre, quel module possède quoi — client, serveur, moteur), relire le code déjà livré autour, et en sortir trois listes : **à refactorer / à supprimer / à ajouter**. La suppression se propose au même titre que l'ajout.
4. **Préférer ce que la bibliothèque fait déjà à sa reconstruction.** Avant d'écrire un mécanisme maison (timer, file d'événements, sérialisation, gestion de fenêtre), vérifier ce que SFML, Asio ou la STL fournissent.
5. **Quand plusieurs itérations dégradent** : revenir à l'état d'origine (git) et repartir de l'étape 2, jamais empiler une correction de plus.

### Une question sur un détail n'autorise pas une refonte

1. **Répondre d'abord**, avec le constat — sans toucher au code.
2. **Ne changer que le détail signalé.**
3. **Une convergence de conventions se propose**, elle ne se livre pas : « ces deux modules divergent sur X, je peux aligner — tu veux ? ».
4. Une réutilisation repérée en chemin est un **finding à signaler**, pas un chantier à ouvrir dans le même commit.

## Exemple

- ❌ **Avant (incorrect)** : la boucle de jeu dérive → écrire un timer maison à base de `sleep_for`, puis le corriger trois fois sur des valeurs devinées.
- ✅ **Après (correct)** : lire la doc de `sf::Clock` et d'`asio::steady_timer`, poser un pas de temps fixe avec accumulateur, et supprimer le timer maison.
