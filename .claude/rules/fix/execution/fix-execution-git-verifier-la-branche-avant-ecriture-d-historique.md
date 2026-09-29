---
description: Toute commande git qui écrit l'historique relit la branche courante dans le même appel ; jamais de mutation git derrière une commande faillible non liée
trigger: always_on
---

# RULE : Toute commande git qui ÉCRIT de l'historique relit la branche courante dans le MÊME appel

## Règle à appliquer

1. **Toute commande git qui écrit de l'historique** — `commit`, `commit --amend`, `rebase`, `reset`, `cherry-pick`, `merge`, `push` — est précédée, **dans le même appel**, d'une lecture de la branche courante, et ne part que si elle correspond :
   ```sh
   [ "$(git branch --show-current)" = "feat/packet-reader" ] || { echo "WRONG BRANCH"; exit 1; }
   git commit --amend --no-edit
   ```
2. **Ne jamais enchaîner une commande git mutante derrière une commande faillible sur une ligne séparée** (heredoc, script, `cd`) en comptant sur un `&&` placé ailleurs : le `&&` ne protège que ce qui est à sa gauche dans la même commande. Séparer en deux appels, lire le résultat du premier, puis lancer le second.
3. **Préférer `git -C <dir>`** à `cd <dir>` suivi d'une commande : il ne dépend d'aucun répertoire courant.
4. **Jamais `--amend` avec rien de staged** sans le vouloir explicitement : vérifier `git status --short` d'abord.
5. **Quand `git log` ne montre pas ce qu'on attend, lire `git reflog`** avant toute autre commande.
6. **Pousser sans checkout** quand le working tree appartient à quelqu'un d'autre : `git push origin ma-branche:ma-branche`.
7. **Réparer sa propre casse ≠ écraser le travail d'autrui** : prouver l'équivalence avant de réaligner une ref, sinon demander.

## Exemple

- ❌ **Avant (incorrect)** :
  ```sh
  python3 fix_includes.py
  git add -A && git commit --amend --no-edit
  ```
  → le script échoue, l'amend part quand même, sur une branche qui a changé entre-temps.
- ✅ **Après (correct)** : un appel pour le script, dont on lit le code de sortie ; puis un second appel qui vérifie la branche et l'index avant d'amender.
