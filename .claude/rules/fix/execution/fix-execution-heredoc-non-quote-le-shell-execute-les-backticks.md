---
description: Un heredoc non quoté exécute les backticks et $() de son texte — tout heredoc qui porte de la prose ou du code se quote (<<'EOF')
trigger: always_on
---

# RULE : Un heredoc non quoté exécute les backticks de son texte — tout heredoc qui porte de la prose ou du code se quote

## Règle à appliquer

1. **Tout heredoc se quote par défaut** : `<<'EOF'`. Le corps part alors tel quel, backticks et `$` compris. C'est le cas en particulier pour un message de commit ou une description de PR en Markdown, pleins de `` `code` ``.
2. **Les valeurs du shell entrent par les arguments**, jamais par interpolation du corps : `python3 - "$file" <<'PY'` puis `sys.argv[1]`.
3. **Un heredoc non quoté ne porte que du texte relu caractère par caractère** — jamais de Markdown, de code, ni de prose qui cite des commandes.
4. **Une commande citée pour l'utilisateur ne doit pas pouvoir s'exécuter en passant** : si elle est destructive, la décrire en mots plutôt que l'écrire exécutable dans un script.
5. **Après un incident de ce type**, mesurer l'effet avant de continuer, le dire en tête de réponse, puis réparer.

## Exemple

- ❌ **Avant (incorrect)** : `gh pr create --body "$(cat <<EOF … run \`cmake --build build --target format\` … EOF)"` → le shell exécute la commande citée et la description perd le texte.
- ✅ **Après (correct)** : `gh pr create --body-file /tmp/pr-body.md`, fichier écrit sans passer par le shell, ou `<<'EOF'`.
