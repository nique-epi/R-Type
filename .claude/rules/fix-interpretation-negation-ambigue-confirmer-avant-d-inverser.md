---
description: Une consigne ambiguë dont une lecture inverse un comportement existant se confirme avant d'implémenter
trigger: always_on
---

# RULE : Une consigne ambiguë qui pourrait inverser un comportement existant se confirme avant d'implémenter

## Règle à appliquer

Devant une consigne dont une lecture **inverse un comportement existant** et l'autre le **confirme** :

1. **Énumérer les clauses et chercher une lecture qui les rend toutes vraies.** Une clause qu'il faut qualifier de « lapsus » pour sauver une lecture est le signal qu'on tient la mauvaise.
2. **Seulement s'il n'existe aucune lecture totale**, le début de la phrase prime : si la consigne ouvre sur un impératif clair, la subordonnée ambiguë se lit dans son sens.
3. **À ambiguïté persistante, poser la question d'une ligne avant de coder.** Confirmer coûte dix secondes ; inverser à tort coûte une PR à défaire.
4. **Ne jamais inverser un comportement livré sur la seule foi d'une phrase ambiguë** : l'existant est un signal d'intention.
5. **Une lecture qui supprime un mécanisme livré exprès se confirme avant le premier `git rm`.** La lecture qui le conserve est la lecture par défaut.
6. **Signaler une ambiguïté avant de livrer, pas après** : un doute posé au-dessus d'une implémentation déjà écrite ne rachète pas le code.
7. **Une donnée transverse ne se tranche pas en local** : avant de changer un sens, un ordre ou une convention (axe Y, unité de vitesse, ordre des octets), chercher qui d'autre la lit.

## Exemple

- ❌ **Avant (incorrect)** : « les ennemis ne doivent pas tirer quand ils ne sont pas à l'écran, sauf le boss » → lu « seul le boss tire » → tous les ennemis à l'écran cessent de tirer.
- ✅ **Après (correct)** : lecture qui rend toutes les clauses vraies : « hors écran, personne ne tire sauf le boss ; à l'écran, rien ne change » — et, au moindre doute, une question d'une ligne.
