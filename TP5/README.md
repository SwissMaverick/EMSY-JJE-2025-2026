# Partie 1

## 1. Analyse du code fourni (structure typique du TP5 EMSY)
### 1.a — Où les données sont écrites dans le FIFO ?
Il y a deux interruptions, chacune écrivant dans le même FIFO :

(1) Interruption Timer (priorité 3) — toutes les 100 ms
Lit la température via SPI

Forme une trame (ex. Txxx\r\n)

Écrit cette trame dans le FIFO

(2) Interruption UART RX (priorité 4)
Se déclenche à chaque caractère reçu

Lit U1RXREG

Forme une trame (ex. Cxx\r\n ou juste le caractère brut)

Écrit ce caractère dans le FIFO

Les deux ISR écrivent dans le même FIFO, ce qui est volontaire pour illustrer les problèmes de concurrence.

### 1.b — Quel est le format des données ? (Codage des trames)
Typiquement dans ce TP :

Trame température
Code
Txxx\n
où xxx = température en dixièmes de degrés.

Trame caractère reçu
Code
C<caractère>\n
ou parfois juste :

Code
<caractère>
Point important
Les deux sources écrivent des trames de longueurs différentes, mais dans un seul FIFO byte-par-byte.

1.c — Où les données sont relues ?
Dans la tâche de fond (main loop) :

Code
while(1) {
    if (!FIFO_empty()) {
        byte = FIFO_read();
        parser(byte);
    }
}
Cette tâche :

lit les octets du FIFO

reconstruit les trames

déclenche l’affichage LCD

### 1.d — Où et comment les données sont décodées ?
Dans la fonction de parsing, typiquement :

Décodage température
détecte un 'T'

lit les 3 chiffres suivants

convertit en entier

affiche sur LCD

Décodage caractère
détecte 'C'

lit le caractère suivant

l’affiche sur LCD

Erreurs typiques détectées
trame incomplète (FIFO vidé trop tôt)

mélange de trames (ex : T12C3T4)

caractères perdus

débordement du FIFO → corruption

### 1.e — Ce qui est affiché
Sur le LCD :

la température mise à jour toutes les 100 ms

les caractères reçus via UART

## 2. Fonctionnement normal (envoi manuel via PuTTY)
Quand tu tapes lentement :

LED0 clignote → lecture température (Timer)

LED1 clignote → réception UART

LED2 clignote → affichage LCD

Le FIFO n’est jamais saturé → tout fonctionne.

## 3. Envoi de plusieurs dizaines de caractères d’un coup : dysfonctionnement
### 3.a — Est-ce systématique ?
Oui.
Dès que tu envoies plus de ~30–50 caractères d’un coup, le FIFO déborde.

### 3.b — Analyse temporelle avec LED0/LED1/LED2
Tu vas observer :

LED1 (UART RX) clignote frénétiquement → réception massive

LED0 (Timer) continue toutes les 100 ms → ajoute encore des trames

LED2 (affichage) ne suit plus → la tâche de fond est trop lente

Résultat :

le FIFO se remplit plus vite qu’il ne se vide

les trames se mélangent

certaines sont tronquées

parfois le FIFO déborde → perte de données

### 3.c — Mise en évidence du dysfonctionnement et cause
Cause principale :
👉 Un seul FIFO partagé entre deux producteurs rapides et un consommateur lent.

Plus précisément :

L’UART reçoit beaucoup plus vite que la tâche de fond ne peut afficher.

Le Timer ajoute encore des trames toutes les 100 ms.

Le FIFO finit par déborder.

Les trames deviennent corrompues :

mélange température / caractères

trames incomplètes

caractères perdus

Le parser reçoit des séquences invalides → affichage incohérent.

Cause secondaire :
Pas de protection contre l’accès concurrent au FIFO.

Même si les interruptions sont prioritaires, l’écriture simultanée peut provoquer :

écriture interrompue en plein milieu

trames mélangées

### 3.d — Solutions possibles
Voici les solutions classiques (et attendues dans ce TP) :

#### Solution 1 — Deux FIFO séparés
FIFO_Temp pour le Timer

FIFO_UART pour la réception UART

La tâche de fond lit les deux FIFO indépendamment

Plus de mélange de trames  
Plus de corruption

#### Solution 2 — FIFO plus grand
Augmenter la taille (ex. 256 ou 512 bytes)

retarde le problème
ne le supprime pas

#### Solution 3 — Protéger le FIFO (section critique)
Désactiver les interruptions pendant l’écriture :

Code
unsigned int status = disableInterrupts();
FIFO_write(...);
restoreInterrupts(status);
évite les trames corrompues
n’empêche pas le débordement

## Mesures du code démo



<img width="1280" height="824" alt="scrprint" src="https://github.com/user-attachments/assets/91ec40b8-72f8-4c60-8102-4280ae9aa1e4" />


## Erreur présente

### a

### b

### c

### d



