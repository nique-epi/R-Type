---
description: Un nouvel appel à un helper existant reprend les gardes que tous ses appelants actuels portent
trigger: always_on
---

# RULE : Un nouvel appel à un helper existant reprend les gardes que ses appelants portent déjà

## Règle à appliquer

Avant d'ajouter un appel à une fonction existante — surtout privée, surtout dans un type qui orchestre un état asynchrone (session réseau, partie, boucle de jeu) :

1. **Lire tous les appelants actuels** (`grep -rn`) et relever ce que chacun vérifie **avant** l'appel. Une garde que tous portent est une précondition du helper, même si rien ne l'écrit.
2. **La reprendre**, ou écrire dans le doc-comment du helper pourquoi le nouveau site en est dispensé.
3. Se demander **quand** le nouveau site s'exécute par rapport aux autres (avant la fin du handshake ? pendant l'arrêt du serveur ? depuis un autre thread ?).
4. Si la précondition mérite d'être garantie plutôt que répétée, la déplacer **dans** le helper — en vérifiant qu'aucun appelant ne comptait sur son absence.

## Exemple

- ❌ **Avant (incorrect)** : `sendSnapshot(clientId)` appelé depuis le nouveau handler de reconnexion, sans le `if (session.isHandshakeComplete())` que portent les deux autres appelants → snapshot envoyé à un client qui ne connaît pas encore son id.
- ✅ **Après (correct)** : la même garde, ou la vérification déplacée dans `sendSnapshot` avec un retour explicite.
