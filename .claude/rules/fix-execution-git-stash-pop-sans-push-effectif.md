---
description: Ne jamais enchaîner git stash push et git stash pop — un push qui ne sauvegarde rien fait dépiler la remise d'avant ; préférer ne pas stasher
trigger: always_on
---

# RULE : Ne jamais enchaîner `git stash push` et `git stash pop` — un push qui ne sauvegarde rien fait dépiler la remise d'AVANT

## Règle à appliquer

1. **Préférer ne pas stasher du tout.** La pile est partagée par tous les worktrees : ce qu'on dépile n'est pas forcément à soi. Pour découper un commit, utiliser l'index (`git add -- <fichiers>` puis `git commit`, deux fois). Pour changer de base, `git rebase origin/main` ou `git merge --ff-only` échouent proprement au lieu de détruire.
2. **Si un stash est indispensable**, ne jamais enchaîner `push` et `pop` dans une commande composée : lancer `git stash push -m "<nom>"`, **lire sa sortie**, vérifier avec `git stash list` que `stash@{0}` porte bien ce nom, et seulement alors dépiler.
3. **Vérifier la pile avant tout dépilage** : `git stash list`.
4. **Après un `pop` en conflit** : `git reset --hard <ref>` annule le dépilage et Git conserve l'entrée. Les fichiers non suivis restaurés survivent au reset : les identifier (`git stash show --include-untracked --name-only stash@{0}`) et les retirer un par un, jamais par un `git clean -fd` aveugle.
5. **Jamais `2>/dev/null || true`** sur une commande git qui écrit.

## Exemple

- ❌ **Avant (incorrect)** : `git stash push README.md && git reset --hard origin/main && git stash pop` → le push ne sauvegarde rien, le pop dépile une vieille remise, conflits.
- ✅ **Après (correct)** : committer le travail sur sa branche, puis `git rebase origin/main` — aucune pile, aucun risque.
