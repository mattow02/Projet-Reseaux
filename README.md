# Network Simulation

![C](https://img.shields.io/badge/C-00599C?style=for-the-badge&logo=c&logoColor=white)
![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20macOS-lightgrey?style=for-the-badge)

A network simulator written in C that models a local network with switches and stations. Reads topology from configuration files, simulates Ethernet frame forwarding, dynamic MAC address learning, and the Spanning Tree Protocol (STP) to prevent switching loops.

---

> **Coursework.** Networking project (SAE) of the BUT Informatique at IUT
> Robert Schuman, Strasbourg. Written in 2025, over about two months.

> **Coursework.** Networking project (SAE) of the BUT Informatique at IUT
> Robert Schuman, Strasbourg. Written in 2025, over about two months.

## Features

- **Topology parsing**, reads `.lan` config files describing switches, stations, and links.
- **MAC address learning**, switches dynamically build forwarding tables from observed traffic.
- **Ethernet frame handling**, create, parse, and display Ethernet frames with proper MAC/IP addressing.
- **Spanning Tree Protocol**, full STP implementation: root bridge election, port roles (Root/Designated/Blocked), loop prevention.
- **Forwarding simulation**, frames are forwarded, flooded, or dropped based on the learned MAC tables and STP port states.

---

## Building

```bash
git clone https://github.com/mattow02/Projet-Reseaux-v2.git
cd Projet-Reseaux-v2
make
```

Le binaire est écrit dans `out/network_sim`. La compilation passe en `-Wall -Wextra` sans un seul avertissement.

## Running

```bash
./out/network_sim mylan.lan.txt
```

Where `mylan.lan.txt` is a topology configuration file. Several examples are included in the repo (`mylan.lan.txt`, `mylan_no_cycle.lan`, `w_mylan_1.lan`, `w_mylan_2.lan`).

---

## Configuration format

A `.lan` file describes the network topology:

```
# Define switches and their ports
switch S1 3
switch S2 3

# Define stations
station PC1 aa:bb:cc:dd:ee:01 192.168.1.1
station PC2 aa:bb:cc:dd:ee:02 192.168.1.2

# Define links between devices
link S1:0 S2:0
link S1:1 PC1
link S2:1 PC2
```

---

## Project structure

```
src/
├── main.c              Entry point, loads config, runs simulation
├── config_reader.c/h   Topology file parser
├── network.c/h         Switch tables, MAC learning, frame forwarding
├── stp.c/h             Spanning Tree Protocol implementation
├── ethernet_frame.c/h  Ethernet frame construction and display
├── addresses.c/h       MAC and IPv4 address utilities
└── types.h             Core data types (MAC, IPv4, graph)
```

---

## How it works

1. The config file is parsed into an in-memory graph of switches and stations.
2. STP runs to elect a root bridge and assign port roles, blocking redundant paths.
3. Ethernet frames are injected into the network.
4. Switches learn source MAC addresses and forward/flood frames according to their tables.
5. The simulation outputs forwarding decisions and final MAC tables for each switch.
