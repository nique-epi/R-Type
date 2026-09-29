---
description: Face à une consigne ambiguë par faute de frappe, retenir la lecture qui applique la règle projet, jamais celle qui la suspend
trigger: always_on
---

# RULE : Face à une consigne ambiguë par typo, retenir la lecture qui SERT la règle projet — jamais celle qui la suspend

## Règle à appliquer

1. **Une consigne ambiguë qui pourrait suspendre une règle se lit dans le sens qui APPLIQUE la règle.** Appliquer la règle pour rien coûte un `git fetch` ; la suspendre à tort coûte un rebase et un force-push.
2. **Un mot mal orthographié qui change le sens se lève avant d'agir**, en une ligne.
3. **Ne jamais écrire une supposition comme un fait** dans un commit, une PR ou `.context/context.md`. Une interprétation s'annonce comme telle, ou se fait confirmer.
4. **`git fetch origin main` avant toute création de branche**, quoi qu'on ait cru lire : c'est en lecture seule et ça ne casse rien.

## Exemple

- ❌ **Avant (incorrect)** : « Par de main à jour » lu « Pas de main à jour » → branche créée sans fetch, partie d'un `main` en retard d'une PR mergée.
- ✅ **Après (correct)** : `git fetch origin main` (coût nul), et une question d'une ligne si le doute persiste.
