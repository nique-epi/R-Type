---
description: Les classes et fichiers structurels se nomment d'après la donnée ou le domaine qu'ils servent, jamais d'après un processus ou du jargon
trigger: always_on
---

# RULE : Les artefacts structurels se nomment d'après la DONNÉE / le DOMAINE qu'ils servent, jamais d'après un processus ou du jargon

## Règle à appliquer

1. **Un type qui porte ou stocke une donnée se nomme d'après cette donnée** : `EntityRegistry`, `PlayerInput`, `Snapshot`, `ClientSession`. Jamais d'après le processus qui l'utilise (`SyncData`, `NetworkStuff`) ni d'après son consommateur.
2. **Les noms de processus sont réservés aux artefacts de processus** : un système qui applique le mouvement peut s'appeler `MovementSystem`, un type qui sérialise `SnapshotSerializer` — il *est* le processus. Il consomme `Snapshot`, il ne s'appelle pas `SnapshotData`.
3. **Pas de suffixe fourre-tout** (`Manager`, `Handler`, `Helper`, `Utils`, `Data`, `Info`) quand un terme précis existe : `ClientRegistry` plutôt que `ClientManager`, `PacketDispatcher` plutôt que `PacketHandler` s'il route vers des handlers.
4. **Pas de préfixe redondant avec le namespace ou le dossier** : `rtype::network::Packet`, pas `rtype::network::NetworkPacket`.
5. Test : un coéquipier qui découvre le dépôt comprend ce que contient le fichier à partir de son nom seul.

## Exemple

- ❌ **Avant (incorrect)** : `NetworkManager.hpp` qui contient à la fois le socket, la liste des clients et la sérialisation ; `GameData.hpp` pour l'état d'une partie.
- ✅ **Après (correct)** : `UdpServer.hpp` (socket), `ClientRegistry.hpp` (clients), `SnapshotSerializer.hpp` (sérialisation), `GameState.hpp` (état de la partie).
