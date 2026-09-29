---
description: Une assertion décrit le comportement voulu, jamais la sortie observée ; un double de test conserve les propriétés risquées de la vraie dépendance
trigger: always_on
---

# RULE : Une assertion décrit le comportement VOULU, jamais le comportement observé — et un double plus sage que la vraie dépendance rend une classe de bugs invisible

Pourquoi : une assertion recopiée de la sortie transforme le comportement courant en spécification — elle passe le jour où on l'écrit et tous les jours suivants, y compris quand le comportement est faux. Et un double de test définit ce que la suite peut voir : toute propriété du réel qu'il ne reproduit pas devient un angle mort commun à toute la suite.

## Règle à appliquer

1. **Une assertion énonce ce que le produit exige, pas ce que le code a renvoyé.** Avant d'écrire `EXPECT_EQ(x, valeur)`, répondre à « pourquoi cette valeur est-elle la bonne ? ». Si la seule réponse est « c'est ce que ça sort », l'assertion ne s'écrit pas.
2. **Se méfier des assertions qui reconnaissent une forme plutôt qu'une valeur** (`EXPECT_FALSE(buffer.empty())`, `EXPECT_GT(size, 0)`) : elles passent pour toute une famille de valeurs, dont les fausses. Quand la valeur attendue est connue, asserter l'**égalité avec la source**.
3. **Un correctif qui fait rougir un test existant est un signal.** Avant d'ajuster le test, se demander s'il gardait le bug ; si oui, sa réécriture fait partie du correctif et **se dit dans la PR**.
4. **Un double de test conserve les propriétés risquées de la vraie dépendance.** Un transport UDP perd, duplique et réordonne des datagrammes : le faux transport doit pouvoir le faire. Une horloge avance : la fausse horloge doit avancer. Un double plus déterministe que le réel est un choix à justifier.
5. **Ne pas modifier un double partagé pour un cas particulier** : injecter un double local qui reproduit la propriété manquante.
6. **Écrire le test, le voir échouer sur le code fautif, puis le garder.** Un test ajouté sur du code déjà corrigé ne prouve pas qu'il attrape quoi que ce soit.
7. **Corollaire de revue** : dans un diff de test, chercher les assertions qui décrivent la mécanique interne plutôt qu'un fait observable par le joueur ou par le protocole.

## Exemple

- ❌ **Avant (incorrect)** :
  ```cpp
  EXPECT_EQ(serialize(snapshot).size(), 37);
  ```
  (37 est ce que le code a sorti ; personne ne sait pourquoi c'est juste.)
- ✅ **Après (correct)** :
  ```cpp
  EXPECT_EQ(deserialize(serialize(snapshot)), snapshot);
  ```
  plus un test du récepteur sous un faux transport qui **réordonne** les datagrammes, pour asserter qu'un snapshot plus ancien n'écrase jamais un plus récent.
