---
description: Un défaut visible par le joueur ne se classe jamais mineur de ma seule autorité ; un arbitrage qui en laisse un se présente, il ne s'entérine pas
trigger: always_on
---

# RULE : Un défaut VISIBLE ne se classe jamais « mineur » de ma seule autorité — un arbitrage qui en laisse un derrière lui se REMONTE

Pourquoi : la sévérité technique (« c'est une ligne ») n'est pas la sévérité produit (« l'écran ment au joueur »).

## Règle à appliquer

1. **« Mineur » n'est pas un mot que je pose seul sur un défaut visible** par le joueur : un affichage faux (score, points de vie, position d'une entité, joueur fantôme, son qui ne se déclenche pas) n'est jamais mineur par défaut. Je peux dire « techniquement petit » ; la sévérité produit appartient à l'utilisateur.
2. **Quand un arbitrage laisse un défaut derrière lui, il se présente, il ne s'entérine pas** : exposer les options avec leurs défauts résiduels respectifs, et laisser trancher. « Je diffère sciemment » n'existe pas sans accord explicite.
3. **Une question sur un périmètre (« on doit le faire là ? ») est une demande d'analyse, pas un mandat de découpe.** J'y réponds par les conséquences de chaque branche, pas par un choix.
4. **Avant d'accepter un défaut résiduel, chercher le design qui ne le produit pas.** Si le conflit vient de deux règles que j'ai posées (coût réseau vs exactitude), le signaler comme un conflit de conception à résoudre.

## Exemple

- ❌ **Avant (incorrect)** : « Je n'envoie pas l'état des points de vie dans le snapshot. Conséquence assumée : la barre de vie peut être fausse jusqu'au prochain dégât. Défaut mineur, différé. »
- ✅ **Après (correct)** : « Deux options. (a) Les points de vie partent dans chaque snapshot : barre toujours juste, +2 octets par entité et par tick. (b) Ils ne partent qu'au changement : snapshot plus léger, mais la barre sera fausse pour un joueur qui rejoint en cours de partie. Je recommande (a). Tu tranches. »
