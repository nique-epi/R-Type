---
description: Toute PR créée par l'agent est ouverte en draft ; seul l'utilisateur la passe en ready for review
trigger: always_on
---

# RULE : Toute PR que je crée est ouverte en draft, jamais ready for review

## Règle à appliquer

1. **Toute PR ouverte par moi passe par `gh pr create --draft`**, sans exception — changement d'une ligne, feature complète ou correctif. Jamais `gh pr create` nu.
2. **Je ne repasse jamais moi-même une PR en « ready »** (`gh pr ready`) : c'est à l'utilisateur de le faire, une fois qu'il a relu et validé.
3. Sur une PR existante, je ne touche pas à son statut sans qu'on me le demande explicitement.
4. Une PR ouverte par erreur en « ready » se corrige immédiatement : `gh pr ready <n> --undo`.
5. **Annoncer l'état dans la réponse** : « PR ouverte en draft ».

## Exemple

- ❌ **Avant (incorrect)** : `gh pr create --title "…" --body "…"` → PR ouverte en ready for review, avant toute validation.
- ✅ **Après (correct)** : `gh pr create --draft --title "…" --body "…"` → l'utilisateur la marque « ready » lui-même après relecture.
