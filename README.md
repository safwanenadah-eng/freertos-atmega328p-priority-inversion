# 🚀 FreeRTOS Priority Inversion & Mutex Demo (ATmega328P)

![FreeRTOS](https://img.shields.io/badge/FreeRTOS-V7.1.0-blue.svg)
![Microcontroller](https://img.shields.io/badge/MCU-ATmega328P-orange.svg)
![Toolchain](https://img.shields.io/badge/Toolchain-AVR--GCC%20%7C%20Proteus-green.svg)

## 📌 Présentation du projet
Ce projet implémente et valide expérimentalement le mécanisme d'**Héritage de Priorité (Priority Inheritance)** sous **FreeRTOS** sur un microcontrôleur 8-bits AVR (ATmega328P / Arduino Uno) simulé sous Proteus.

L'objectif est de démontrer comment un **Mutex** FreeRTOS résout le problème critique d'**Inversion de Priorité**, un phénomène redouté dans la conception des systèmes embarqués temps réel applicatifs.

---

## 🌍 Applications Réelles & Contexte Industriel

L'inversion de priorité survient lorsqu'une tâche haute priorité est indirectement bloquée par une tâche moyenne priorité. Dans un système temps réel critique (Hard Real-Time), ce retard peut provoquer une perte de contrôle ou un crash.

### 1. Le Cas Historique : Mars Pathfinder (1997) 🔴
La mission martienne de la NASA a failli être un échec total à cause d'une inversion de priorité sur l'OS temps réel VxWorks :
- **Tâche Faible (Météo) :** Acquérait un bus de données partagé.
- **Tâche Haute (Gestion d'Information) :** Devait lire ce bus avant de réinitialiser le chien de garde (*Watchdog*).
- **Tâche Moyenne (Communications) :** A préempté la tâche météo, empêchant la tâche haute d'accéder au bus.
- **Résultat :** Le Watchdog expirait et le robot redémarrait sans cesse.
- **Solution appliquée à distance :** Activation du mécanisme d'**Héritage de Priorité** sur le Mutex du bus.

### 2. Cas d'Usage Industriels Modernes 🛠️
- **Automobile (Systèmes ADAS / Freinage ABS) :** Garantir qu'un capteur de freinage d'urgence (`High Priority`) accède immédiatement au bus CAN même si un calcul de télémétrie (`Low Priority`) possède la ressource.
- **Aéronautique & Avionique :** Empêcher les flux de données secondaires d'interrompre les boucles de contrôle de vol.
- **Dispositifs Médicaux (Respirateurs, Pousse-Seringues) :** Garantir que les alarmes vitales préemptent les routines d'affichage de l'écran tactile.

---

## 🛠️ Architecture Logicielle & Démonstration

Le système se compose de trois tâches concurrentes partageant un Mutex unique (`xResourceMutex`) :

| Tâche | Priorité | Rôle |
| :--- | :---: | :--- |
| **`vTaskLow`** | `1` (Faible) | Verrouille le Mutex et exécute un traitement long (Section Critique). |
| **`vTaskMedium`** | `2` (Moyenne) | Tâche périodique indépendante (ne demande pas le Mutex). |
| **`vTaskHigh`** | `3` (Haute) | Tâche prioritaire demandant le Mutex détenu par `vTaskLow`. |

### 🔄 Le Mécanisme d'Héritage de Priorité en Action
1. `vTaskLow` prend le Mutex et commence son traitement.
2. `vTaskHigh` se réveille et demande le Mutex $\rightarrow$ Bloquée car indisponible.
3. **Action de FreeRTOS :** La priorité de `vTaskLow` est automatiquement **boostée temporairement au niveau 3**.
4. `vTaskMedium` (Priorité 2) ne peut plus interrompre `vTaskLow`.
5. `vTaskLow` libère le Mutex $\rightarrow$ Sa priorité redevient `1`, et `vTaskHigh` prend la main **immédiatement**.

---

## 💻 Validation Expérimentale (Virtual Terminal)

![Résultat du Terminal](assets/terminal_demo.png)

### Chronologie des logs observés :
```text
=== FreeRTOS Priority Inversion Demo ===

[LOW] Demande Mutex...
[LOW] Mutex Acquis ! Debut travail...
[LOW] Travail Termine. Liberation Mutex...
    [HIGH] Demande Mutex...
    [HIGH] EXECUTION AVEC HERITAGE PRIO !
  [MEDIUM] Tache Moyenne tourne