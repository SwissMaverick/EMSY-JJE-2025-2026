# Partie 1

## Analyse du code:

### a 

Dans les interruptions

### b

void APP_SendCharToApp(int8_t c)
{
    int8_t frame[9];
    uint8_t i;
    
    //formatte trame
    frame[0] = 2;   //début de trame
    frame[1] = '2'; //type de trame
    frame[2] = c;
    ComputeChecksum(&frame[1], 2, &frame[3]);   //remplit checksum (frame[3] et [4])
    frame[5] = 3;   //fin de trame
    frame[6] = 0;   //padding pour trame totale 9 car.
    frame[7] = 0;   //padding pour trame totale 9 car.
    frame[8] = 0;   //padding pour trame totale 9 car.
            
    //place trame dans fifo
    for(i=0 ; i<9 ; i++)
        PutCharInFifo(&fifoLcd, frame[i]);
}

void APP_SendTempToApp(float fTemp)
{
    int8_t frame[9];
    uint8_t i;
    char sTemp[5];
    
    sprintf(sTemp, "%4.1f", (double)fTemp);
    
    //formatte trame
    frame[0] = 2;   //début de trame
    frame[1] = '1'; //type de trame
    frame[2] = sTemp[0];
    frame[3] = sTemp[1];
    frame[4] = sTemp[2];
    frame[5] = sTemp[3];
    ComputeChecksum(&frame[1], 5, &frame[6]);   //remplit checksum (frame[3] et [4])
    frame[8] = 3;   //fin de trame
    
    //place trame dans fifo
    for(i=0 ; i<9 ; i++)
        PutCharInFifo(&fifoLcd, frame[i]);    
    
}

### c

uint8_t GetMessage(void)

### d

//analyse le buffer de réception
uint8_t GetMessage(void)
{
    uint8_t nbCar;
    char car;
    uint8_t retVal = 0; //par défaut pas de message reçu complet
    
    #define MSG_BUF_SIZE 9
    static char msgBuf[9];
    static uint8_t msgBufCntr = 0;
    
    nbCar = GetReadSize(&fifoLcd);
    
    if (nbCar > 0) //qqchose à lire ?
    {
        while (nbCar>0/* && msgBufCntr<MSG_BUF_SIZE*/)
        {
            GetCharFromFifo(&fifoLcd, (int8_t*)&car);
            nbCar--;

            if (car != 0) //les caractères 0 passent à la trappe (padding de trame qu'il n'y a pas besoin de décoder)
            {
                msgBuf[msgBufCntr] = car;
                if (msgBufCntr<MSG_BUF_SIZE)                
                    msgBufCntr++; 

                if (car == 0x03) //fin de trame reçue ?
                {          
                    //décodage...
                    if (msgBufCntr==9 && msgBuf[0]==0x02 && msgBuf[1]=='1') //message de température ? (contrôle début de trame, type et longueur)   
                    {
                        if (CheckChecksum((int8_t*)&msgBuf[1], 5, (int8_t*)&msgBuf[6]))//contrôle checksum
                        {
                            appData.newTemp[0] = msgBuf[2];
                            appData.newTemp[1] = msgBuf[3];
                            appData.newTemp[2] = msgBuf[4];
                            appData.newTemp[3] = msgBuf[5];
                            appData.newTemp[4] = 0; //fin de chaine
                            msgBufCntr = 0; //vide buffer pour recommencer message prochain
                            retVal = 1;                            
                        }
                        else
                        {
                            appData.nbErrors++;
                            msgBufCntr = 0; //vide buffer pour recommencer message prochain
                            retVal = 3;     
                        }               
                    }
                    else if (msgBufCntr==6 && msgBuf[0]==0x02 && msgBuf[1]=='2') //message de caractère ? (contrôle début de trame,type et longueur) 
                    {
                        if (CheckChecksum((int8_t*)&msgBuf[1], 2, (int8_t*)&msgBuf[3]))//contrôle checksum
                        {
                            appData.newChar = msgBuf[2];
                            msgBufCntr = 0; //vide buffer pour recommencer message prochain
                            retVal = 2;                            
                        }
                        else
                        {
                            appData.nbErrors++;
                            msgBufCntr = 0; //vide buffer pour recommencer message prochain
                            retVal = 3;     
                        }                 
                    }
                    else
                    {
                        appData.nbErrors++;
                        msgBufCntr = 0; //vide buffer pour recommencer message prochain
                        retVal = 3;    
                    }                                   
                }
            }
        } 
    }

    return(retVal);
}

### e

switch (GetMessage())  
            {
                case 1: //reçu une température
                {
                    lcd_gotoxy(1,3);
                    printf_lcd("t: ");
                    printf_lcd("%s", appData.newTemp); 
                    lcd_gotoxy(1,4);
                    printf_lcd(" OK / %4d erreurs", appData.nbErrors);
                    break;
                } 
                case 2: //reçu un caractère
                {
                    lcd_gotoxy(10,3);
                    printf_lcd("/ car: %c", appData.newChar);
                    lcd_gotoxy(1,4);
                    printf_lcd(" OK / %4d erreurs", appData.nbErrors);
                    break;
                }  
                case 3: //erreur
                {
                    lcd_gotoxy(1,4);
                    printf_lcd("NOK / %4d erreurs", appData.nbErrors);
                    break;
                }
                default:
                {
                    //rien reçu
                    break;
                }
            }

## Mesures du code démo



<img width="1280" height="824" alt="scrprint" src="https://github.com/user-attachments/assets/91ec40b8-72f8-4c60-8102-4280ae9aa1e4" />


## Erreur présente

### a

### b

### c

### d



