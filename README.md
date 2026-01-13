# KivaDB v1.0.0

KivaDB est un moteur de stockage clé-valeur (Key-Value Store) haute performance et léger, écrit en C. Il utilise une architecture de stockage **Log-Structured (Append-Only)** couplée à un **Index Hash Map en mémoire** pour un accès ultra-rapide.

## 🚀 Fonctionnalités
- **Stockage Append-Only** : Performance d'écriture maximale grâce aux entrées-sorties (I/O) séquentielles.
- **Recherche Rapide** : Indexation par table de hachage en RAM pour une complexité de lecture en $O(1)$.
- **Persistance des Données** : Les données sont sauvegardées sur disque et rechargées automatiquement au démarrage.
- **Outils de Maintenance** : 
  - `stats` : Surveille la santé de la base et la fragmentation.
  - `compact` : Libère l'espace disque en supprimant les entrées obsolètes.
- **Sécurité** : Système de verrouillage (locking) pour éviter la corruption par plusieurs processus.

## 📁 Structure du Projet
- `src/core/` : Cœur du moteur (Stockage, Indexation, Transactions).
- `src/cli/` : Interface en ligne de commande (Shell) et tests de performance.
- `include/` : En-têtes (headers) de l'API publique.

## 🛠️ Installation et Compilation
Ce projet est conçu pour être compilé avec `gcc` (recommandé : MinGW ou w64devkit sous Windows).

1. Cloner le dépôt :
   ```bash
   git clone [https://github.com/fomadev/KivaDB.git](https://github.com/fomadev/KivaDB.git)
   cd KivaDB

2. Compiler le projet :
    ```bash
    make
    ```

## 🎮 Utilisation
**Shell Interactif** <br>
Lancez le shell pour gérer vos données manuellement :
```bash
./kivadb_cli.exe
```
**Commandes disponibles :**
* `set <clé> <valeur>` : Stocker une donnée.

* `get <clé>` : Récupérer une valeur.

* `del <clé>` : Supprimer une clé.

* `scan` : Lister toutes les clés actives.

* `stats` : Afficher la taille et l'état de santé du fichier.

* `compact` : Nettoyer le fichier disque (défragmentation).

## 📊 Performances (v1.0.0)
Résultats observés sur stockage SSD :

* Vitesse d'écriture : ~38 000+ opérations/sec (avec Buffered I/O).

* Vitesse de lecture : Quasi-instantanée (recherche en mémoire vive).

📄 <a href="LICENSE">Licence
MIT License</a>