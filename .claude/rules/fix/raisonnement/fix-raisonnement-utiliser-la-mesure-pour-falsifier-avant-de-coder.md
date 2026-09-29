---
description: Quand une mesure est disponible, chiffrer la prédiction de chaque hypothèse et éliminer celles qu'elle réfute avant de coder
trigger: always_on
---

# RULE : Utiliser la mesure pour falsifier les hypothèses avant de coder

## Règle à appliquer

Quand l'utilisateur fournit une mesure (capture, chiffre, timing, log horodaté, profil) :

1. **Chiffrer la prédiction de chaque hypothèse** avant d'écrire une ligne. Hypothèse A prédit 16 ms par frame, hypothèse B prédit 33 ms, la mesure dit 33 ms → A est morte, sans rien compiler.
2. **Vérifier que l'hypothèse retenue explique aussi le code actuel** : si elle n'explique pas pourquoi le bug existe déjà, il manque un facteur.
3. Quand plusieurs hypothèses survivent et qu'on ne peut pas trancher, **livrer le correctif correct sous TOUTES les hypothèses survivantes**, jamais celui qui n'est juste que sous la préférée.
4. **« Je n'ai pas pu vérifier » n'est pas un disclaimer qui autorise à deviner** : c'est le signal qu'il faut chercher la donnée manquante, ou appliquer le point 3.

## Exemple

- ❌ **Avant (incorrect)** : « le jeu tourne à 30 FPS, c'est le rendu qui est lent » → optimiser le rendu → aucun changement.
- ✅ **Après (correct)** : un rendu lent prédirait un temps de frame variable ; la mesure dit 33,3 ms stable → c'est une limite, pas une lenteur → trouver le `setFramerateLimit(30)` ou la vsync.
