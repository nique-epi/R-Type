---
description: Un compte, un total ou une liste produit par un filtre maison se valide sur des cas connus avant d'être rapporté ou de piloter une suppression
trigger: always_on
---

# RULE : Un compte ne vaut que si l'instrument qui l'a produit a été validé sur un cas connu

## Règle à appliquer

Avant de rapporter un **compte, un total ou une liste** produit par un filtre maison (`grep`, regex, `awk`, `jq`, script) :

1. **Valider l'instrument sur un cas connu** : choisir deux ou trois éléments qui *doivent* apparaître — de préférence les plus atypiques (casse mixte, espaces, templates, déclarations sur plusieurs lignes) — et vérifier qu'ils sont dans la sortie. S'ils manquent, c'est l'instrument qui est faux.
2. **Croiser avec un total indépendant** (`wc -l`, nombre de fichiers, compte brut). Un écart inexpliqué est le signal.
3. **Rendre la liste, pas seulement le nombre.** Une liste s'inspecte d'un coup d'œil ; un nombre non.
4. **Se méfier des classes de caractères restrictives** (`[A-Z_]`, `\w`) sur des identifiants réels.

### Un filtre non validé ne pilote jamais une suppression

1. Compter faux coûte un chiffre faux ; supprimer faux coûte le travail. Avant toute action destructive dérivée d'un filtre, valider l'instrument.
2. **L'analyse statique par regex ne voit pas tous les usages** (macros, templates, appels via pointeur de fonction, enregistrement par nom) : une recherche d'« inutilisé » est un minorant de l'usage. La mesure fiable est le **build** et les **tests** après suppression.
3. Committer avant de supprimer en masse.

## Exemple

- ❌ **Avant (incorrect)** : `grep -rn "void on[A-Z]" src | wc -l` → « 12 handlers » ; les handlers déclarés sur deux lignes et les lambdas n'ont jamais été vus.
- ✅ **Après (correct)** : vérifier que deux handlers connus (dont un multi-ligne) sont dans la sortie, élargir le motif, rendre la liste complète.
