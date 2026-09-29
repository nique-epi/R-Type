---
description: Tout livrable (plan, rapport, review, résumé, PR) s'ouvre par le contexte que le lecteur n'a pas encore dans cette conversation
trigger: always_on
---

# RULE : Poser le CONTEXTE avant le livrable — jamais un résumé qui suppose acquis ce dont on n'a jamais parlé

## Règle à appliquer

1. **Tout livrable — plan, rapport, review, résumé, description de PR — s'ouvre par un rappel de contexte de 2 à 5 lignes**, dès que le sujet n'a pas été discuté auparavant **dans cette conversation** : de quoi on parle, où (client, serveur, moteur, build), l'état actuel, ce qui ne va pas.
2. **Le critère est la conversation, pas le dépôt.** Un fait découvert dans un appel d'outil de ce tour n'est pas acquis pour l'utilisateur : il ne l'a pas lu.
3. **Le contexte se dit en mots, pas en références.** `PacketReader.cpp:42` n'est pas du contexte, c'est une **preuve** ; elle vient après l'énoncé, jamais à sa place.
4. **Nommer les choses en clair** : « le serveur ignore les paquets d'entrée du deuxième joueur », pas « `handleInput` return early ». Le nom du symbole entre parenthèses si utile.
5. **Une correction se contextualise deux fois** : rappeler ce qui avait été affirmé, puis ce que j'ai mesuré.
6. **Test de relecture** : un lecteur qui n'a rien vu de mes appels d'outils comprend-il le premier paragraphe ? Sinon, réécrire l'ouverture.

## Exemple

- ❌ **Avant (incorrect)** : « `ClientRegistry::find` renvoie `end()` pour l'endpoint IPv6 mappé (ClientRegistry.cpp:31). Je normalise dans `UdpServer`… »
- ✅ **Après (correct)** : « **Le problème.** Sur Windows, le deuxième joueur se connecte mais ne peut pas bouger : le serveur ne reconnaît pas l'adresse d'où viennent ses paquets, parce qu'elle arrive sous une autre forme (IPv4 encapsulée en IPv6) qu'au moment de la connexion. Le détail mesuré est plus bas. »
