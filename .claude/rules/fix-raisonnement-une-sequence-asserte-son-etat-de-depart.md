---
description: Chaque étape d'une séquence demande un état absolu et vérifie son résultat ; elle ne suppose jamais où la précédente l'a laissée
trigger: always_on
---

# RULE : Une séquence à plusieurs étapes ASSERTE son état de départ — elle ne suppose jamais où la précédente l'a laissée

## Règle à appliquer

1. **Toute étape nomme l'état qu'elle exige, en absolu** : `setState(GameState::Playing)`, jamais `nextState()`. Idempotent : déjà dans l'état demandé ⇒ succès immédiat.
2. **La première étape ré-asserte**, même quand l'initialisation a « déjà fait le travail » : l'initialisation tourne une fois, la séquence autant de fois qu'elle est rejouée (nouvelle partie, reconnexion, niveau suivant).
3. **Une transition qui échoue interrompt la séquence** : elle rend un résultat que l'appelant teste, pas un `void` avec un log d'erreur pendant que la séquence continue.
4. **Rejouer est le cas nominal.** Devant tout état retenu entre deux exécutions (partie en cours, file de paquets, compteur de séquence), se demander : *à quoi ressemble la deuxième exécution si la première s'est arrêtée au milieu ?*
5. **Une inversion silencieuse est pire qu'un échec** : quand deux valeurs de même type ne se distinguent que par leur provenance (source/destination, ancien/nouveau, client/serveur), garantir la provenance par le type ou la construction.

## Exemple

- ❌ **Avant (incorrect)** : `toggleReady()` à chaque clic sur « Prêt » → un paquet perdu inverse l'état entre client et serveur, pour le reste de la partie.
- ✅ **Après (correct)** : le client envoie `setReady(true)` / `setReady(false)`, état absolu, et le serveur renvoie l'état appliqué.
