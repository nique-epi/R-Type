---
description: En zsh, $VAR:qqchose déclenche les modifiers d'expansion — écrire les specs git rev:path en littéral
trigger: always_on
---

# RULE : En zsh, `$VAR:chemin` déclenche les MODIFIERS d'expansion (`:s`, `:h`, `:t`…)

## Règle à appliquer

1. **Ne jamais écrire `$VAR:qqchose` nu en zsh.** Pour un spec git `rev:path`, écrire le littéral : `git show "origin/main:src/server/main.cpp"`. Les modifiers s'appliquent aussi entre guillemets ; si une variable est inévitable, placer le `:` hors de l'expansion : `git show "$rev":"$file"`.
2. **Un `git show` qui affiche un en-tête de commit** alors qu'on attendait un contenu de fichier est le symptôme de ce bug (le chemin a été avalé). Ne jamais raisonner sur cette sortie.
3. Tout idiome shell venu d'un réflexe bash se vérifie pour zsh avant d'interpréter son résultat.

## Exemple

- ❌ **Avant (incorrect)** : `base=$(git merge-base HEAD origin/main); git show $base:CMakeLists.txt` → zsh applique un modifier → sortie incohérente.
- ✅ **Après (correct)** : `git show "$(git merge-base HEAD origin/main)":CMakeLists.txt` → contenu du fichier.
