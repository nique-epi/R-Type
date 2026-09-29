---
description: Une valeur que l'utilisateur qualifie de bouche-trou se remplace par une valeur dérivée ou possédée par la bonne source, jamais reconduite
trigger: always_on
---

# RULE : Un hardcode que l'utilisateur signale comme bouche-trou doit être remplacé, pas conservé

## Règle à appliquer

Quand l'utilisateur qualifie lui-même une valeur en dur de pis-aller et demande « mieux », la passe doit la **remplacer par une valeur dérivée de sa vraie source** — taille de la fenêtre, taille réelle de la texture, dimensions du niveau, valeur calculée à partir d'une autre constante — jamais la reconduire telle quelle ou la renommer seulement. Le test : qui possède le nombre après la passe ? Si la réponse est encore « un littéral choisi à la main », la passe n'est pas finie.

## Exemple

- ❌ **Avant (incorrect)** : l'utilisateur dit « le `1920` en dur pour la largeur de l'écran, c'est provisoire » → je le passe en constante `screenWidth = 1920` et je considère le point réglé.
- ✅ **Après (correct)** : la largeur est lue sur la vue SFML (`window.getView().getSize().x`) au moment où on en a besoin ; plus aucun `1920` dans le code.
