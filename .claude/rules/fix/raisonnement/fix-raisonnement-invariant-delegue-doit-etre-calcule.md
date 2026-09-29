---
description: Un invariant qu'on fait asserter par un test ou un sous-agent doit être calculé avant d'être écrit dans le brief
trigger: always_on
---

# RULE : Un invariant qu'on fait asserter doit être CALCULÉ avant d'être écrit dans un brief — jamais paraphrasé de mémoire

## Règle à appliquer

1. **Tout invariant qu'un brief demande d'ASSERTER dans un test doit d'abord être CALCULÉ** — petit programme jetable, test exploratoire, exécution sur le vrai code — jamais dérivé de mémoire ni recopié d'un raisonnement antérieur. Si je fais écrire `EXPECT_EQ(a, b)`, j'ai exécuté `a` et `b` et je connais leurs valeurs.
2. **Un slogan de conception n'est pas une spécification de test.** « La sérialisation est symétrique », « le tick est déterministe », « c'est idempotent » sont des résumés pour un humain. Avant de les faire asserter, les traduire en mécanique exacte (quelle fonction, quelle entrée, quelle sortie) et vérifier que la traduction tient.
3. **Si le brief ne peut pas porter la valeur calculée, il porte l'ordre de la calculer** : « dérive l'invariant depuis le code et mesure-le avant d'asserter », jamais « l'invariant est X, asserte X ». Un agent à qui on donne un faux invariant comme acquis le forcera au lieu de le questionner.
4. **Quand un agent conteste un invariant de mon brief, mesurer avant de trancher.** Jamais réaffirmer le brief par autorité.

## Exemple

- ❌ **Avant (incorrect)** : brief → « deux simulations avec la même graine produisent le même état au tick 100 ; écris le test qui l'asserte » — sans l'avoir vérifié (les ennemis tirent sur `std::random_device`).
- ✅ **Après (correct)** : lancer deux simulations → constater la divergence au tick 12 → brief → « la simulation n'est pas déterministe aujourd'hui (source : `EnemySpawner` utilise `std::random_device`). Rends-la déterministe par injection du générateur, puis asserte l'égalité au tick 100. »
