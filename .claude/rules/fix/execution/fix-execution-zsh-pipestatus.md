---
description: Ne jamais lire le code de sortie d'une commande à travers un pipe — rediriger vers un fichier et lire $? immédiatement
trigger: always_on
---

# RULE : Récupérer le code de sortie d'une commande pipée en zsh (pas `PIPESTATUS`)

## Règle à appliquer

1. **Ne jamais juger du succès d'une commande à travers un pipe.** Le `$?` après un pipe est celui de la **dernière** commande (`tail`, `grep`), pas de celle qu'on teste.
2. Pour obtenir le vrai code : lancer la commande **sans pipe**, rediriger la sortie vers un fichier, lire `$?` immédiatement, puis filtrer le fichier :
   ```bash
   cmake --workflow --preset test > /tmp/test.log 2>&1; echo "exit: $?"
   tail -30 /tmp/test.log
   ```
3. Si une lecture de statut de pipe est indispensable en zsh : `$pipestatus[1]` (minuscules, indexé à 1), jamais `${PIPESTATUS[0]}`.
4. Un gate n'est annoncé vert qu'avec **la sortie et le code** lus correctement.

## Exemple

- ❌ **Avant (incorrect)** : `cmake --build build 2>&1 | tail -15; echo $?` → affiche 0 même quand la compilation échoue.
- ✅ **Après (correct)** : `cmake --build build > /tmp/build.log 2>&1; echo "exit: $?"` → vrai code (non nul en échec), puis lecture du log.
