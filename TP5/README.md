# TP5 — Observation du fonctionnement sans OS

## Objectif
Analyser l’application PIC32 fournie : deux ISR (Timer, UART RX) écrivent dans un **FIFO unique** ; une tâche de fond lit le FIFO et affiche sur LCD. Mesurer le comportement temporel via trois LEDs (CH1/CH2/CH3) et diagnostiquer le dysfonctionnement observé lors d’un envoi massif via PuTTY.

---

# 1. Analyse du code fourni (structure typique)

## 1.a — Où les données sont écrites dans le FIFO
- **ISR Timer (prio 3, 100 ms)**  
  - Lecture SPI température  
  - Forme trame `Txxx\r\n`  
  - Écrit octet‑par‑octet dans le FIFO
- **ISR UART RX (prio 4)**  
  - Se déclenche par caractère reçu (`U1RXREG`)  
  - Forme trame `C<caractère>\r\n` ou écrit le caractère brut  
  - Écrit octet‑par‑octet dans le même FIFO

> Les deux ISR partagent le même FIFO (conception volontaire pour illustrer concurrence).

---

## 1.b — Format des données (codage)
- **Température** : `Txxx\n` (`xxx` = dixièmes de °C)  
- **Caractère** : `C<char>\n` ou `<char>`  
- **Remarque** : trames de longueurs différentes stockées byte‑wise dans un FIFO commun.

---

## 1.c — Où les données sont relues
Tâche de fond (main loop) :
```c
while (1) {
  if (!FIFO_empty()) {
    byte = FIFO_read();
    parser(byte);
  }
}
```

---

## 1.d — Décodage et erreurs détectées
- **Décodage température** : détecte 'T', lit 3 chiffres, convertit, affiche.

- **Décodage caractère** : détecte 'C', lit caractère suivant, affiche.

- **Erreurs observées** : trame incomplète, mélange de trames (T12C3T4), caractères perdus, débordement FIFO → corruption.

---

## 1.e — Affichage
- **LCD affiche** : température (toutes les 100 ms) et caractères UART.

---

## 2. Fonctionnement normal (tests manuels PuTTY)
**Envoie caractère unique** : pas de saturation.

LED mapping :

CH1 / LED0 = tâche de fond (apptask)

CH2 / LED1 = UART RX ISR

CH3 / LED2 = Timer ISR (lecture température)

Observation : LED1 et LED2 ISR courtes, LED0 s’exécute moins fréquemment pour vider FIFO.

---

# 3. Envoi massif (copier‑coller) — dysfonctionnement
## 3.a — Fréquence
Systématique au-delà d’environ 30–50 caractères envoyés d’un coup.

## 3.b — Comportement temporel (LEDs)
CH2 (UART) : rafales très denses.

CH3 (Timer) : impulsions périodiques (100 ms).

CH1 (apptaski) : exécution lente, ne suit pas le débit entrant.

## 3.c — Cause
FIFO unique : producteurs (UART rapide + Timer périodique) > consommateur (tâche de fond).

Pas de protection/section critique : écritures concurrentes peuvent interrompre une écriture en cours → trames mélangées.

Résultat : débordement FIFO, trames corrompues, affichage incohérent.

## 3.d — Solutions (priorisées)
Deux FIFO séparés (FIFO_Temp, FIFO_UART) — isolation des flux.

Protéger écritures FIFO (désactiver interruptions brièvement) :

c
unsigned int s = disableInterrupts();
FIFO_write(...);
restoreInterrupts(s);
Augmenter taille FIFO (256–512 bytes) — mitigation temporaire.

Réduire fréquence Timer (100 ms → 500 ms) si acceptable.

Migrer vers RTOS (FreeRTOS) : une queue par producteur/consommateur, gestion priorités.

# 4. Mesures oscilloscope — où et comment
4.1 Emplacements sondes
CH1 (LED0) → broche LED0 (tâche de fond / apptaski)

CH2 (LED1) → broche LED1 (UART RX ISR)

CH3 (LED2) → broche LED2 (Timer ISR)

## 4.2 Réglages recommandés
Couplage : DC

Vertical : 2 V/div

Timebase : 10 ms/div (pour rafales) ou 100 ms/div (vue longue)

Trigger : CH2 (UART) en edge rising, single/normal pour capturer rafales

Probes : 10x, masse commune correctement reliée

## 4.3 Procédure de mesure
Flasher le firmware.

Connecter sondes aux broches LED (éviter boucles de masse).

Configurer scope selon réglages.

Scénarios :

Test A : saisie lente via PuTTY (baseline).

Test B : copier‑coller massif (déclenchement du dysfonctionnement).

Capturer écrans et sauvegarder captures pour analyse.

## 4.4 Interprétation rapide
CH2 : rafales → producteur dominant.

CH3 : impulsions périodiques → charge additionnelle.

CH1 : impulsions espacées → consommateur lent.

Corrélation temporelle montre FIFO se remplit plus vite qu’il ne se vide.


Conclusion (résumé technique)
Problème principal : FIFO partagé entre ISR rapides et tâche lente → débordement et corruption.

Correctifs prioritaires : séparer flux, protéger accès, RTOS pour solution robuste.

Mesures oscillo (CH1=apptaski, CH2=UART ISR, CH3=Timer ISR) confirment la surcharge et la dynamique producteur/consommateur.
