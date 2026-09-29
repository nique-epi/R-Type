---
description: Poser les questions fermées qui coupent l'arbre des hypothèses avant d'écrire un protocole de diagnostic
trigger: always_on
---

# RULE : Demander ce que l'utilisateur observe déjà AVANT de bâtir un protocole de diagnostic

## Règle à appliquer

1. Avant d'écrire un protocole de diagnostic, un runbook ou une instrumentation large, **poser d'abord les une à trois questions fermées qui coupent l'arbre des hypothèses en deux** — et attendre la réponse. Typiquement : « à quelle étape ça s'arrête ? », « est-ce que X se produit, oui ou non ? », « ça a déjà marché ? », « sur Linux, Windows, ou les deux ? ».
2. On instrumente ensuite **la moitié de l'arbre qui reste**, pas l'arbre entier. Un protocole dont l'utilisateur peut rayer la moitié des étapes de tête est écrit trop tôt.
3. Quand le symptôme est décrit en une phrase, **reformuler ce qu'on en comprend** et le faire confirmer avant de partir sur des étapes coûteuses (rebuild complet des dépendances vcpkg, réinstallation d'outils).

## Exemple

- ❌ **Avant (incorrect)** : « le client ne se connecte pas » → runbook en six étapes dont « supprime `build/`, reconfigure, vérifie le pare-feu ».
- ✅ **Après (correct)** : « le serveur affiche-t-il la connexion entrante ? » → « oui » → le problème est côté réponse : instrumenter seulement l'envoi de l'acquittement et sa réception.
