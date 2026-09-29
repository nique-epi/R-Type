---
description: git checkout -b part de HEAD — toujours nommer origin/main comme point de départ et vérifier la topologie après création et après push
trigger: always_on
---

# RULE : `git checkout -b` part de HEAD, pas de `main` — nommer le point de départ et vérifier la topologie

## Règle à appliquer

1. **Toujours nommer explicitement le point de départ**, sur la ref distante fraîche :
   ```bash
   git fetch origin main
   git checkout -b <type>/<slug> origin/main
   ```
   `origin/main` ne dépend ni de HEAD, ni de l'état du `main` local.
2. **Vérifier la topologie juste après la création**, avant tout commit :
   ```bash
   git log --oneline origin/main..HEAD   # must be empty on a fresh branch
   ```
3. **Pousser en créant sa propre ref amont** : `git push -u origin <type>/<slug>` (une branche créée depuis `origin/main` le suit par défaut ; un `git push` nu viserait `main`).
4. **Après le push, vérifier la PR, pas la commande** :
   ```bash
   gh pr view <n> --json state,mergeable,commits
   ```
   `MERGEABLE` et le nombre de commits attendu.
5. **Réparation** : ne jamais rebaser/force-pusher depuis le working tree de l'utilisateur ; monter un `git worktree add --detach <tmp> origin/main`, y cherry-picker les commits légitimes, `push --force-with-lease`, retirer le worktree.

## Exemple

- ❌ **Avant (incorrect)** : sur `ci/build-and-test`, `git checkout -b feat/logger` → la branche embarque les commits de la CI, PR avec des commits parasites.
- ✅ **Après (correct)** : `git checkout -b feat/logger origin/main` → `git log --oneline origin/main..HEAD` vide → commit → `git push -u origin feat/logger` → `gh pr view` : 1 commit.
