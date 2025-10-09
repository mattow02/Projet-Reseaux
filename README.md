SCHNIDER-SYLVESTRE Martin | MARTIN Josué | STROHER Matteo

# Rapport Final du Projet SAE S21 - Simulation de Réseau et Algorithme STP

## 1. Équipe de Développement

### 1.1 Membres de l'Équipe
- **SCHNIDER-SYLVESTRE Martin**
- **MARTIN Josué**
- **STROHER Matteo**

### 1.2 Organisation du Travail
Le projet a été réalisé par une équipe de 3 personnes, permettant une répartition efficace des tâches et une collaboration étroite sur les différents aspects du développement.

---

## 2. Objectif et Présentation Générale

Ce projet a pour but de simuler un réseau local composé de switches et de stations, de lire une configuration réseau depuis un fichier, d’afficher les tables de commutation, et de simuler l’algorithme Spanning Tree Protocol (STP) pour éviter les boucles de commutation.  
Une partie du projet permet également de manipuler et d’afficher des trames Ethernet, ainsi que de simuler l’apprentissage dynamique des adresses MAC par les switches.

---

## 3. Architecture des fichiers

### 3.1 Structure du Projet

```
sae-s21-reseau/
├── src/
│   ├── addresses.c/h        # Manipulation et affichage des adresses MAC/IP
│   ├── config_reader.c/h    # Lecture et gestion de la configuration réseau
│   ├── ethernet_frame.c/h   # Gestion et affichage des trames Ethernet
│   ├── network.c/h          # Structures principales du réseau, tables de commutation, apprentissage MAC
│   ├── stp.c/h              # Implémentation du protocole STP
│   ├── types.h              # Types de base (MAC, IPv4, graphe)
│   └── main.c               # Point d'entrée du programme et logique de test
├── out/                     # Fichiers objets
├── exe/                     # Exécutables
└── Makefile                 # Compilation
```

---

## 4. Détail des Structures et Modules Importants

### 4.1 Types de base (`types.h`)
- `mac_address_t` : adresse MAC (6 octets)
- `ipv4_address_t` : adresse IPv4 (4 octets)
- Structures pour la représentation de graphes (topologie du réseau)

### 4.2 Gestion des adresses (`addresses.c/h`)
- Affichage et comparaison d’adresses MAC et IPv4

### 4.3 Configuration réseau (`config_reader.c/h`)
- Lecture d’un fichier de configuration décrivant les switches, stations et liens
- Structures :  
  - `link_t` : lien entre équipements (indices, coût)
  - `network_config_t` : configuration globale (tableaux de switches, stations, liens)

### 4.4 Structures principales du réseau et apprentissage MAC (`network.c/h`)
- `station_t` : une station du réseau
- `switch_t` : un switch avec sa table de commutation dynamique
- `switch_table_entry_t` : entrée de la table de commutation (MAC, port)
- Fonctions :
  - `learn_mac` : apprentissage dynamique de la MAC source sur le port d’entrée
  - `receive_frame_on_switch` : simule la réception d’une trame et l’apprentissage MAC
  - `print_switch_tables` : affichage des tables de commutation

### 4.5 Trames Ethernet (`ethernet_frame.c/h`)
- `ethernet_frame_t` : structure complète d’une trame Ethernet (préambule, SFD, adresses, type, données, FCS)
- Fonctions d’affichage lisible et hexadécimal

### 4.6 Protocole STP (`stp.c/h`)
- Structures pour la simulation du protocole STP :  
  - `stp_switch_t`, `stp_port_t`, `stp_network_t`
- Fonctions pour :  
  - Initialiser le réseau STP
  - Élire le pont racine
  - Calculer les chemins pour éviter les boucles
  - Libérer la mémoire

---

## 5. Fonctionnement du Programme

### 5.1 Lecture et affichage de la configuration
Le programme lit un fichier de configuration et affiche la topologie du réseau ainsi que les tables de commutation de chaque switch.

### 5.2 Initialisation et exécution du STP
Le réseau STP est initialisé, le pont racine est élu, puis les chemins sont calculés pour éviter les boucles.

### 5.3 Simulation de l’apprentissage MAC par les switches
Deux trames sont créées :
- Une trame envoyée de Switch 0 vers Switch 1 (source = MAC de S0, destination = MAC de S1)
- Une trame envoyée de Switch 1 vers Switch 0 (source = MAC de S1, destination = MAC de S0)

À chaque réception, la fonction `receive_frame_on_switch` est appelée, ce qui permet au switch destinataire d’apprendre automatiquement l’adresse MAC source sur le port d’entrée (simulation du comportement réel d’un switch Ethernet).

### 5.4 Affichage des tables de commutation
Après les échanges, les tables de commutation sont affichées : chaque switch a appris l’adresse MAC de l’autre switch sur le port correspondant.

### 5.5 Nettoyage mémoire
Toute la mémoire allouée est libérée proprement à la fin de l’exécution.

---

## 6. Détail des Structures Principales

### 6.1 `mac_address_t` et `ipv4_address_t`
Définissent respectivement une adresse MAC (6 octets) et une adresse IPv4 (4 octets).

### 6.2 `switch_t` et `switch_table_entry_t`
- `switch_t` : représente un switch avec sa MAC, ses ports, et sa table de commutation dynamique.
- `switch_table_entry_t` : entrée de la table (MAC, port).

### 6.3 `ethernet_frame_t`
Représente une trame Ethernet complète (voir `ethernet_frame.h`).

### 6.4 Structures STP
- `stp_switch_t` : switch STP, contient un pointeur vers le switch réel, ses ports, et des infos STP
- `stp_port_t` : état d’un port STP (racine, désigné, bloqué, coût)
- `stp_network_t` : ensemble des switches STP

---

## 7. Explications sur les Parties Importantes du Code

### 7.1 Simulation de l’apprentissage MAC (`main.c`)
```c
// Trame de Switch 0 vers Switch 1
ethernet_frame_t frame1;
memset(&frame1, 0, sizeof(ethernet_frame_t));
for (int i = 0; i < 7; i++) frame1.preambule[i] = 0xAA;
frame1.sfd = 0xAB;
memcpy(&frame1.src, &config->network.switches[0].mac, sizeof(mac_address_t));
memcpy(&frame1.dest, &config->network.switches[1].mac, sizeof(mac_address_t));
frame1.type = 0x0800;
strcpy((char*)frame1.data, "De S0 à S1");
frame1.data_len = strlen((char*)frame1.data);
frame1.fcs = 0x11111111;
print_ethernet_frame(&frame1);
receive_frame_on_switch(&config->network.switches[1], &frame1, 0); // Switch 1 apprend la MAC de S0

// Trame de Switch 1 vers Switch 0
ethernet_frame_t frame2;
memset(&frame2, 0, sizeof(ethernet_frame_t));
for (int i = 0; i < 7; i++) frame2.preambule[i] = 0xAA;
frame2.sfd = 0xAB;
memcpy(&frame2.src, &config->network.switches[1].mac, sizeof(mac_address_t));
memcpy(&frame2.dest, &config->network.switches[0].mac, sizeof(mac_address_t));
frame2.type = 0x0800;
strcpy((char*)frame2.data, "De S1 à S0");
frame2.data_len = strlen((char*)frame2.data);
frame2.fcs = 0x22222222;
print_ethernet_frame(&frame2);
receive_frame_on_switch(&config->network.switches[0], &frame2, 0); // Switch 0 apprend la MAC de S1
```

### 7.2 Affichage des tables de commutation
```c
print_switch_tables(&config->network);
```
Chaque switch affiche la MAC source apprise sur le port d’entrée.

---

## 8. Points forts du projet

- **Modularité** : chaque aspect du réseau (adresses, configuration, STP, trames, apprentissage MAC) est séparé dans un fichier dédié.
- **Simulation réaliste** : l’apprentissage dynamique des adresses MAC fonctionne comme sur un vrai switch.
- **Lisibilité** : de nombreuses fonctions d’affichage facilitent la compréhension de l’état du réseau.
- **Respect des standards** : structures et algorithmes proches de la réalité (STP, trame Ethernet).

---

## 9. Limites et Travail non terminé

Nous n'avons pas pu terminer la phase 4 du projet : seule la moitié de cette phase a été réalisée.  
Certaines fonctionnalités prévues, notamment la gestion complète de la dynamique du spanning tree ou des cas avancés de simulation, n'ont pas pu être finalisées dans les temps.

---

## 10. Améliorations Possibles

- Ajouter la gestion dynamique des tables de commutation (apprentissage MAC sur plusieurs ports et avec stations)
- Simuler le passage effectif des trames dans tout le réseau (forwarding complet)
- Ajouter une interface graphique ou web pour visualiser la topologie et l’évolution du spanning tree

---

## 11. Conclusion

Ce projet met en œuvre les concepts fondamentaux des réseaux locaux, du protocole STP et de l’apprentissage dynamique des adresses MAC, tout en permettant de manipuler des trames Ethernet et de visualiser l’état du réseau à chaque étape.  
La modularité du code et la séparation claire des responsabilités facilitent la maintenance et l’évolution du projet.