---
description: Créer la branche de travail depuis origin/main avant le premier commit — une feature = une branche = une PR
trigger: always_on
---

# RULE : Créer la branche AVANT le premier commit — la branche checkoutée n'est jamais un défaut acceptable

## Règle à appliquer

**Avant le PREMIER `git commit` d'une unité de travail — pas avant le push, pas avant la PR :**

1. **Nommer le travail du commit à venir et le comparer à la branche courante.** `git branch --show-current` : si le nom ne décrit pas le travail, **on ne commit pas dessus**. Une branche de feature n'accueille que sa feature ; un correctif découvert en chemin part sur sa propre branche. On ne commit jamais directement sur `main`.
2. **Créer la branche depuis la ref distante fraîche** (cf. `fix-execution-checkout-b-part-de-head-pas-de-main.md`), nommée `<type>/<slug>` comme dans l'historique (`chore/build-system`, `ci/build-and-test`).
3. **Re-vérifier l'état de la branche au moment de committer**, pas en début de session : une branche dont la PR est mergée et la ref distante supprimée est **morte** — tout commit posé dessus est orphelin.
4. **Plusieurs correctifs sans rapport = plusieurs branches.** Le coût d'une branche est nul ; démêler après coup coûte un cherry-pick par commit.
5. **Réparation**, si des commits sont déjà mal placés : ne jamais rebaser ni force-pusher le working tree de l'utilisateur. Monter `git worktree add --detach <tmp> origin/main`, y cherry-picker les commits sur des branches neuves, pousser, retirer le worktree.
6. **Ne jamais supprimer la branche mal utilisée avant que ses commits soient replantés et poussés ailleurs** — c'est la seule copie.

## Exemple

- ❌ **Avant (incorrect)** : sur `feat/entity-registry`, corriger un bug de la CI → commit sur cette branche → la PR du registre embarque un changement de CI sans rapport.
- ✅ **Après (correct)** : `git fetch origin main && git checkout -b ci/fix-cache origin/main` → `git log --oneline origin/main..HEAD` vide → commit, push, PR séparée.
