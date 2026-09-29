---
description: Une branche qui détruit un état se déclenche sur les codes d'erreur précis qui prouvent la condition, jamais sur un type d'erreur
trigger: always_on
---

# RULE : Un type d'erreur n'est pas un diagnostic

## Règle à appliquer

Avant d'écrire une branche qui **détruit un état** (déconnecter un client, fermer une partie, vider une file, réinitialiser une session) :

1. **Restreindre le déclencheur aux codes précis**, jamais au type. Énumérer les codes qui prouvent la condition (`asio::error::connection_refused`, `asio::error::operation_aborted` à l'arrêt…) ; **tout le reste est transitoire par défaut**. Le défaut est toujours « garder l'état » : se tromper dans ce sens coûte un nouvel essai, dans l'autre un joueur éjecté.
2. **Lire le comportement par défaut de l'appel destructif** dans la doc ou les headers avant de l'écrire (que fait `close()` sur les opérations en attente ? quels handlers reçoivent `operation_aborted` ?), et rendre les choix explicites au site d'appel.
3. **Vérifier ce que la couche du dessous fait déjà** avant d'ajouter une gestion par-dessus.
4. **Ne jamais affirmer un comportement d'API dans un doc-comment sans l'avoir vérifié.**

## Exemple

- ❌ **Avant (incorrect)** : `if (error) { disconnectClient(clientId); }` dans le handler de réception UDP — un `connection_refused` transitoire (ICMP d'un autre port) éjecte le joueur.
- ✅ **Après (correct)** : ignorer `operation_aborted` (arrêt volontaire), relancer la réception sur les erreurs transitoires, et ne déconnecter un client que sur l'expiration de son délai d'inactivité — l'UDP n'a pas de connexion à perdre.
