# JJE - LCX
# TP5 — Observation du fonctionnement sans OS

## Objectif
Analyser l’application PIC32 fournie : deux ISR (Timer, UART RX) écrivent dans un **FIFO unique** ; une tâche de fond lit le FIFO et affiche sur LCD. Mesurer le comportement temporel via trois LEDs (CH1/CH2/CH3) et diagnostiquer le dysfonctionnement observé lors d’un envoi massif via PuTTY.

---

# 1. Analyse du code fourni (structure typique)

## 1.a — Où les données sont écrites dans le FIFO
Fonction APP_SendCharToApp — construit une trame caractère puis place chaque octet dans le FIFO via PutCharInFifo.

```c
//place trame dans fifo
for(i=0 ; i<9 ; i++)
    PutCharInFifo(&fifoLcd, frame[i]);
```
Fonction APP_SendTempToApp — construit une trame température puis place chaque octet dans le FIFO via PutCharInFifo.

```c
//place trame dans fifo
for(i=0 ; i<9 ; i++)
    PutCharInFifo(&fifoLcd, frame[i]);
```
Initialisation du FIFO
```c
InitFifo ( &fifoLcd, FIFO_LCD_BUF_SIZE, fifoLcdBuf, 0 );
```
Interprétation : les deux fonctions formatent une trame (9 octets au total) puis écrivent octet par octet dans le FIFO avec PutCharInFifo.

> Les deux ISR partagent le même FIFO (conception volontaire pour illustrer concurrence).

---

## 1.b — Format des données (codage)
Construction observée dans le code :

**Trame température (type '1')**

```c
frame[0] = 2;   // début de trame (0x02)
frame[1] = '1'; // type de trame
frame[2] = sTemp[0];
frame[3] = sTemp[1];
frame[4] = sTemp[2];
frame[5] = sTemp[3];
ComputeChecksum(&frame[1], 5, &frame[6]); // checksum stockée à partir de frame[6]
frame[8] = 3;   // fin de trame (0x03)
```
**Trame caractère (type '2')**

```c
frame[0] = 2;   // début de trame
frame[1] = '2'; // type de trame
frame[2] = c;   // payload 1 octet
ComputeChecksum(&frame[1], 2, &frame[3]); // checksum stockée à partir de frame[3]
frame[5] = 3;   // fin de trame
frame[6] = 0; frame[7] = 0; frame[8] = 0; // padding pour atteindre 9 octets
```
Résumé du protocole :

**Octet 0** : 0x02 (STX) — début de trame.

**Octet 1** : type '1' (température) ou '2' (caractère).

**Payload** : pour '1' → 4 caractères ASCII représentant la température formatée ("%4.1f"), pour '2' → 1 caractère.

**Checksum** : calculé par ComputeChecksum sur frame[1] avec une longueur variable (5 pour temp, 2 pour char) et stocké dans les octets suivants.

**Octet fin** : 0x03 (ETX).

**Padding** : la trame est envoyée sur 9 octets (zéros ajoutés si nécessaire).

---

## 1.c — Où les données sont relues
Où la lecture a lieu :

**Test de données disponibles**

```c
if (GetReadSize(&fifoLcd) > 0)
    appData.state = APP_STATE_SERVICE_TASKS;
```
**Lecture octet par octet dans GetMessage()**

```c
nbCar = GetReadSize(&fifoLcd);
...
GetCharFromFifo(&fifoLcd, (int8_t*)&car);
nbCar--;
```
**Remarque** : GetMessage() lit la taille disponible via GetReadSize() puis récupère chaque octet avec GetCharFromFifo, en ignorant les octets 0 (padding).

---

## 1.d — Décodage et erreurs détectées
```c
uint8_t GetMessage(void)
```
- **Décodage température** : détecte 'T', lit 3 chiffres, convertit, affiche.

- **Décodage caractère** : détecte 'C', lit caractère suivant, affiche.

- **Erreurs observées** : trame incomplète, mélange de trames (T12C3T4), caractères perdus, débordement FIFO → corruption.
  
```c
if (car != 0) // padding ignoré
{
    msgBuf[msgBufCntr] = car;
    if (msgBufCntr<MSG_BUF_SIZE)
        msgBufCntr++;

```
Détection fin de trame et décodage température

```c
if (car == 0x03) // fin de trame
{
    if (msgBufCntr==9 && msgBuf[0]==0x02 && msgBuf[1]=='1') // trame temp attendue
    {
        if (CheckChecksum((int8_t*)&msgBuf[1], 5, (int8_t*)&msgBuf[6]))
        {
            appData.newTemp[0] = msgBuf[2];
            appData.newTemp[1] = msgBuf[3];
            appData.newTemp[2] = msgBuf[4];
            appData.newTemp[3] = msgBuf[5];
            appData.newTemp[4] = 0; // fin de chaîne
            msgBufCntr = 0;
            retVal = 1; // température valide
        }
        else
        {
            appData.nbErrors++;
            msgBufCntr = 0;
            retVal = 3; // erreur checksum
        }
    }
```
Décodage caractère

```c
else if (msgBufCntr==6 && msgBuf[0]==0x02 && msgBuf[1]=='2') // trame caractère
{
    if (CheckChecksum((int8_t*)&msgBuf[1], 2, (int8_t*)&msgBuf[3]))
    {
        appData.newChar = msgBuf[2];
        msgBufCntr = 0;
        retVal = 2; // caractère valide
    }
    else
    {
        appData.nbErrors++;
        msgBufCntr = 0;
        retVal = 3; // erreur checksum
    }
}
else
{
    appData.nbErrors++;
    msgBufCntr = 0;
    retVal = 3; // format inattendu
}
```
Codes de retour de GetMessage() :

0 : pas de message complet.

1 : température valide (payload stocké dans appData.newTemp).

2 : caractère valide (stocké dans appData.newChar).

3 : erreur (checksum ou format incorrect) — incrémente appData.nbErrors.

---

## 1.e — Affichage

Initialisation et messages statiques

```c
lcd_init();
printf_lcd("EMSY3 TP5 FreeRTOS");
lcd_gotoxy(1,2);
printf_lcd("Demo sans OS");
lcd_gotoxy(10,3);
printf_lcd("/ car: -");
lcd_bl_on();
```
Affichage après réception (dans APP_STATE_SERVICE_TASKS)

```c
case 1: // température reçue
    lcd_gotoxy(1,3);
    printf_lcd("t: ");
    printf_lcd("%s", appData.newTemp);
    lcd_gotoxy(1,4);
    printf_lcd(" OK / %4d erreurs", appData.nbErrors);
    break;

case 2: // caractère reçu
    lcd_gotoxy(10,3);
    printf_lcd("/ car: %c", appData.newChar);
    lcd_gotoxy(1,4);
    printf_lcd(" OK / %4d erreurs", appData.nbErrors);
    break;

case 3: // erreur
    lcd_gotoxy(1,4);
    printf_lcd("NOK / %4d erreurs", appData.nbErrors);
    break;
```
**Résumé de l’affichage** :

- Ligne 1–2 : titre et sous‑titre.

- Ligne 3 : affiche la température (t: <valeur>) ou le caractère (/ car: <c>).

- Ligne 4 : affiche le statut (OK ou NOK) et le compteur d’erreurs (appData.nbErrors).

---

## 2. Fonctionnement normal (tests manuels PuTTY)
**Envoie caractère unique** : pas de saturation.

<img width="1280" height="824" alt="Série de caractères" src="https://github.com/user-attachments/assets/1de17cc9-3aeb-40c6-8600-bbf7cecdf71f" />


LED mapping :

CH1 / LED0 = tâche de fond (apptask)

CH2 / LED1 = UART RX ISR

CH3 / LED2 = Timer ISR (lecture température)

Observation : LED1 et LED2 ISR courtes, LED0 s’exécute moins fréquemment pour vider FIFO.

---

# 3. Envoi massif (copier‑coller) — dysfonctionnement

---

## 3.a — Fréquence
Systématique au-delà d’environ 30–50 caractères envoyés d’un coup.

---

## 3.b — Comportement temporel (LEDs)

<img width="1280" height="824" alt="Vue générale" src="https://github.com/user-attachments/assets/0245da1a-a44e-45eb-b680-3d3f5c2e8b79" />

CH1 (apptask) : exécution lente, ne suit pas le débit entrant.

CH2 (UART) : rafales très denses.

CH3 (Timer) : impulsions périodiques (100 ms).

---

## 3.c — Cause
FIFO unique : producteurs (UART rapide + Timer périodique) > consommateur (tâche de fond).

Pas de protection/section critique : écritures concurrentes peuvent interrompre une écriture en cours → trames mélangées.

Résultat : débordement FIFO, trames corrompues, affichage incohérent.

---

## 3.d — Solutions (priorisées)
Deux FIFO séparés (FIFO_Temp, FIFO_UART) — isolation des flux.

Protéger écritures FIFO (désactiver interruptions brièvement) :

```c
unsigned int s = disableInterrupts();
FIFO_write(...);
restoreInterrupts(s);
```
Augmenter taille FIFO — Séparer en deux FIFO.

Migrer vers RTOS (FreeRTOS) : une queue par producteur/consommateur, gestion priorités.

---

# 4. Mesures oscilloscope
---
## 4.1 Emplacements sondes
CH1 (LED0) → broche LED0 (tâche de fond / apptask)

CH2 (LED1) → broche LED1 (UART RX ISR)

CH3 (LED2) → broche LED2 (Timer ISR)

---

## 4.2 Réglages 
Couplage : DC

Vertical : 2 V/div

Timebase : 10 ms/div (pour rafales) ou 100 ms/div (vue longue)

Trigger : CH2 (UART) en edge rising, single/normal pour capturer rafales

---

## 4.3 Procédure de mesure

Putty : serial COMx (RS232 PC) 9600 bit/s

Câble croisé ( null modem ) entre PC et Carte de dévellopement PIC32

Test A : saisie lente via PuTTY .

Test B : copier‑coller massif (déclenchement du dysfonctionnement).

Capturer écrans et sauvegarder captures pour analyse.

---

## 4.4 Interprétation rapide
CH2 : rafales → producteur dominant.

CH3 : impulsions périodiques → charge additionnelle.

CH1 : impulsions espacées → consommateur lent.

Corrélation temporelle montre FIFO se remplit plus vite qu’il ne se vide.

---

# Conclusion 
Problème principal : FIFO partagé entre ISR rapides et tâche lente → débordement et corruption.

Correctifs : séparer flux (deuxième FIFO), RTOS pour solution robuste.

---

# TP5 — Observation du fonctionnement avec OS

# 5. Mesures oscilloscope avec OS

## 5.1 Emplacements sondes
CH1 (LED0) → broche LED0 (App température)

CH2 (LED1) → broche LED1 (App LCD)

CH3 (LED2) → broche LED2 (ISR UART RX)

CH4 (LED3) → broche LED2 (ISR Timer)

---

## 5.2 Réglages 
Couplage : DC

Vertical : 2 V/div

Timebase : 10 ms/div (pour rafales) ou 100 ms/div (vue longue)

Trigger : CHx ( cor.. a remplir ) en edge rising, single/normal pour capturer rafales

---

## 5.3 Procédure de mesure

Putty : serial COMx (RS232 PC) 9600 bit/s

Câble croisé ( null modem ) entre PC et Carte de dévellopement PIC32

Test A : saisie lente via PuTTY.

Test B : copier‑coller massif.

Capturer écrans et sauvegarder captures pour analyse.


---

## 5.4 Fonctionnement normal (tests manuels PuTTY)

**Envoi caractère unique** : pas de saturation.

<img width="1280" height="824" alt="Envoi simple" src="https://github.com/user-attachments/assets/c6ab3acc-e829-4382-b235-7d3af3506907" />

LED mapping : 

CH1 / LED0 =  (App température)

CH2 / LED1 =  (App LCD)

CH3 / LED2 =  (ISR UART RX)

CH4 / LED3 =  (ISR Timer)

Observation : Envoi de deux caractères, chaque flanc correspondant à un caractère. Fonctionnement normal et sans problèmes. 

---

## 5.5 Envoi massif (copier‑coller) — dysfonctionnement

<img width="1280" height="824" alt="Envoi massif" src="https://github.com/user-attachments/assets/4ebac12b-55c0-48a9-be7a-9426e5064470" />

<img width="1280" height="824" alt="Envoi massif (zoom)" src="https://github.com/user-attachments/assets/ad9547fb-1b66-4e62-b84c-f98c3bebe796" />

LED mapping :

CH1 / LED0 =  (App température)

CH2 / LED1 =  (App LCD)

CH3 / LED2 =  (ISR UART RX)

CH4 / LED3 =  (ISR Timer)

Observation : Le programme a le temps de remplir et de vider le FIFO pour envoyer tous les caractères. Aucun conflit entre les deux apps. 

---
# Conclusion ( avec OS)

( a remplir )
