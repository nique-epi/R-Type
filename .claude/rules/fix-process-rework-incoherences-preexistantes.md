---
description: Une incohérence repérée se retravaille ou se signale, même pré-existante — jamais l'étendre en silence
trigger: always_on
---

# RULE : Une incohérence repérée se REWORK, même si elle est pré-existante — jamais l'étendre en silence

Pourquoi : le « minimal diff » porte sur le périmètre fonctionnel, pas sur la qualité. Chaque extension d'un mauvais pattern augmente le coût de sa résorption.

## Règle à appliquer

1. **Toute incohérence repérée** (violation des conventions du dépôt ou des `.claude/rules/` : corps de méthode dans un `.hpp`, `throw std::` brut, valeur magique, nommage hors convention, couche contournée, dépendance SFML côté serveur…) **est traitée, même si elle est antérieure au travail en cours.** « Le fichier faisait déjà comme ça » n'est jamais une justification pour l'étendre.
2. **Évaluer l'impact avant d'agir** :
   - **Impact contenu** (le fichier ou le module courant, comportement identique, prouvable par le build et les tests) → **refactorer directement**, dans un commit `refactor` séparé du changement fonctionnel.
   - **Impact fort** (plusieurs modules, protocole réseau, boucle de jeu, risque de changement de comportement, travail d'un coéquipier en cours dessus) → **STOP, demander**, avec le constat, l'option de rework et son coût.
   - **Doute sur la frontière** → demander. Demander est toujours permis ; étendre en silence ne l'est jamais.
3. **L'incohérence est toujours signalée** dans la PR ou le rapport, même quand le rework est différé sur décision.
4. S'applique aussi aux **sous-agents** : leurs briefs portent cette règle.

## Exemple

- ❌ **Avant (incorrect)** : le fichier définit ses getters dans le `.hpp` → j'ajoute le mien dans le `.hpp` « pour rester cohérent avec le fichier ».
- ✅ **Après (correct, impact contenu)** : je déplace les corps existants dans le `.cpp` (commit `refactor` dédié), puis j'ajoute ma méthode au bon endroit.
- ✅ **Après (correct, impact fort)** : « Le format de paquet duplique ses tailles côté client et serveur. Les unifier touche les deux binaires et le protocole. (a) je le fais maintenant dans un commit dédié, (b) j'ajoute mon paquet a minima et on unifie juste après. Tu préfères ? »
