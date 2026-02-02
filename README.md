# KivaDB 🚀

KivaDB est un moteur de base de données clé-valeur (Key-Value Store) ultra-léger écrit en C, inspiré par l'architecture **Bitcask**. Il utilise un journal d'écriture seule (*Append-only log*) et une table de hachage en mémoire pour garantir des performances de pointe.

## ✨ Nouveautés de la v1.0.3
- **Opérations Groupées (Bulk)** : Traitez plusieurs clés en une seule commande grâce au mot-clé `and`.
- **Analyseur Syntaxique Avancé** : Support robuste des guillemets pour les chaînes contenant des espaces.
- **Performances Certifiées** : Capacité de traitement de +26 000 ops/sec (validé par stress test).
- **Typage Dynamique** : Détection automatique des types `String`, `Number`, et `Boolean`.

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
      <th>Syntaxe Bulk / Avancée</th>
      <th>Description</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>set</td>
      <td><code>set k1 v1 and k2 "valeur complexe"</code></td>
      <td>Crée de nouvelles paires clé-valeur.</td>
    </tr>
    <tr>
      <td>get</td>
      <td><code>get key1 and key2</code></td>
      <td>Récupère les valeurs d'une ou plusieurs clés.</td>
    </tr>
    <tr>
      <td>update</td>
      <td><code>update key1 val1 and key2 val2</code></td>
      <td>Modifie des clés existantes uniquement.</td>
    </tr>
    <tr>
      <td>typeof</td>
      <td><code>typeof &lt;key&gt;</code></td>
      <td>Affiche le type détecté (string, number, boolean).</td>
    </tr>
    <tr>
      <td>del</td>
      <td><code>del key1 and key2</code></td>
      <td>Supprime une ou plusieurs clés de l'index.</td>
    </tr>
    <tr>
      <td>scan</td>
      <td><code>scan</code></td>
      <td>Liste toutes les clés, types et tailles en RAM.</td>
    </tr>
    <tr>
      <td>compact</td>
      <td><code>compact</code></td>
      <td>Défragmente le fichier .kiva pour libérer l'espace.</td>
    </tr>
    <tr>
      <td>stats</td>
      <td><code>stats</code></td>
      <td>Affiche l'état de santé et la fragmentation.</td>
    </tr>
  </tbody>
</table>

## 📈 Performances (Stress Test)
Les tests effectués sur la v1.0.3 montrent des résultats exceptionnels pour un moteur monothread :

* **Volume** : 100 000 entrées.

* **Temps total** : ~3.79 secondes.

* **Vitesse moyenne** : 26 364 ops/sec.

* **Stabilité** : Intégrité des données vérifiée post-insertion massive.

# ⚖️ Licence
Distribué sous licence <a href="LICENSE">MIT</a>.