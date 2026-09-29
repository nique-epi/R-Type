---
description: Quand un changement est annoncé « neutre ici, bénéfique là », nommer l'axe qui diffère, sinon les deux affirmations se contredisent
trigger: always_on
---

# RULE : Quand j'annonce un changement « neutre ici, bénéfique là », NOMMER l'axe qui diffère

## Règle à appliquer

1. **Toute proposition de la forme « ça ne change rien en X mais ça change tout en Y » nomme explicitement l'axe le long duquel X et Y diffèrent**, dans la même phrase.
2. **Quand deux affirmations vraies se lisent comme une contradiction, c'est un défaut de ma démonstration**, pas de la compréhension du lecteur. Ne pas réexpliquer plus fort : identifier la variable cachée et la mettre au premier plan.
3. **Se relire en cherchant la contradiction apparente** avant d'envoyer une justification qui combine « neutre » et « gain important ».
4. Vaut aussi pour les briefs de sous-agents et les descriptions de PR.

## Exemple

- ❌ **Avant (incorrect)** : « Passer le registre en `std::vector` ne change rien aux perfs, le nombre d'entités est petit. Et ça divise par trois le temps du système de collision. »
- ✅ **Après (correct)** : « Le **nombre d'accès** ne change pas : un par entité et par tick, avant comme après. Ce qui change, c'est la **localité mémoire** : en `std::vector`, le système de collision parcourt des composants contigus au lieu de suivre des pointeurs, d'où le gain. »
