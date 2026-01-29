# KivaDB 

KivaDB est un moteur de base de données clé-valeur (Key-Value Store) ultra-léger écrit en C, inspiré par l'architecture Bitcask. Il utilise un journal d'écriture seule (Append-only log) et une table de hachage en mémoire pour des performances optimales.

## ✨ Nouveautés de la v1.0.2
- **Typage Dynamique** : Support automatique des types `String`, `Number`, et `Boolean`.
- **Support des Espaces** : Possibilité d'enregistrer des valeurs avec espaces en utilisant des guillemets (ex: `"Fordi Malanda"`).
- **Interface CLI Améliorée** : Nouvelles commandes `typeof` et `stats`.
- **Arguments CLI** : Support de `-v / --version` et `-h / --help`.

## 🛠️ Installation & Compilation

Assurez-vous d'avoir `gcc` et `make` installés sur votre système.

```bash
git clone https://github.com/votre-compte/kivadb.git
cd KivaDB
make
```
## Utilisation du Shell
Lancez la base de données :
```bash
./kivadb
```

Commandes disponible :
<table border="1">
  <thead>
    <tr>
      <th>Commande</th>
      <th>Description</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>set &lt;key&gt; &lt;val&gt;</code></td>
      <td>Crée une nouvelle clé (utiliser "" pour les espaces).</td>
    </tr>
    <tr>
      <td><code>get &lt;key&gt;</code></td>
      <td>Récupère la valeur associée à une clé.</td>
    </tr>
    <tr>
      <td><code>update &lt;key&gt; &lt;val&gt;</code></td>
      <td>Modifie une clé existante.</td>
    </tr>
    <tr>
      <td><code>typeof &lt;key&gt;</code></td>
      <td>Affiche le type de donnée (string, number, boolean).</td>
    </tr>
    <tr>
      <td><code>del &lt;key&gt;</code></td>
      <td>Supprime une clé de la base.</td>
    </tr>
    <tr>
      <td><code>scan</code></td>
      <td>Liste toutes les clés avec leurs types et tailles.</td>
    </tr>
    <tr>
      <td><code>compact</code></td>
      <td>Défragmente le fichier disque pour gagner de l'espace.</td>
    </tr>
    <tr>
      <td><code>stats</code></td>
      <td>Affiche la santé de la base de données.</td>
    </tr>
  </tbody>
</table>

## 📁 Structure du Projet
* `src/core/ `: Moteur de stockage, indexation et transactions.

* `src/cli/` : Interface en ligne de commande et tests de stress.

* `include/` : Fichiers d'en-tête (API publique).

* `data/` : Dossier contenant le fichier de stockage `.kiva`.

# ⚖️ Licence
Distribué sous licence <a href="LICENSE">MIT</a>.