/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It 
    implements the logic of the application's state machine and it may call 
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************

#include "app.h"
#include "Mc32DriverLcd.h"
#include <stdio.h>  //pr sprintf
#include "checksum.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.
    
    Application strings and buffers are be defined outside this structure.
*/

APP_DATA appData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

uint8_t GetMessage(void);

/* TODO:  Add any necessary local functions.
*/


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT;
    appData.nbErrors = 0;
}


/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks ( void )
{

    /* Check the application's current state. */
    switch ( appData.state )
    {
        /* Application's initial state. */
        case APP_STATE_INIT:
        {
            //init. FIFO
            InitFifo ( &fifoLcd, FIFO_LCD_BUF_SIZE, fifoLcdBuf, 0 );
                    
            //capteur température
            SPI_InitLM70();
            
            //init LCD + affichage message
            lcd_init();
            printf_lcd("EMSY3 TP5 FreeRTOS");
            lcd_gotoxy(1,2);
            printf_lcd("Demo sans OS");
            lcd_gotoxy(10,3);
            printf_lcd("/ car: -");
            lcd_bl_on();
            
            DRV_TMR0_Start();
            
            appData.state = APP_STATE_WAIT;

            break;
        }

        case APP_STATE_WAIT:
        {
            PLIB_INT_SourceDisable(INT_ID_0, INT_SOURCE_TIMER_1);
            PLIB_INT_SourceDisable(INT_ID_0, INT_SOURCE_USART_1_RECEIVE );
            if (GetReadSize(&fifoLcd) > 0)
                appData.state = APP_STATE_SERVICE_TASKS;
            PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_TIMER_1);
            PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_USART_1_RECEIVE );            
            
            break;
        }
        
        case APP_STATE_SERVICE_TASKS:
        {
            
            BSP_LEDToggle(BSP_LED_0);   //debug
            
            //analyse message reçu et affichage
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
        
            appData.state = APP_STATE_WAIT;
            
            break;
        }

        /* TODO: implement your application state machine.*/
        

        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}


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

/*******************************************************************************
 End of File
 */
