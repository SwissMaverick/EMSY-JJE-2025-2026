/*******************************************************************************
 System Interrupts File

  File Name:
    system_interrupt.c

  Summary:
    Raw ISR definitions.

  Description:
    This file contains a definitions of the raw ISRs required to support the
    interrupt sub-system.

  Summary:
    This file contains source code for the interrupt vector functions in the
    system.

  Description:
    This file contains source code for the interrupt vector functions in the
    system.  It implements the system and part specific vector "stub" functions
    from which the individual "Tasks" functions are called for any modules
    executing interrupt-driven in the MPLAB Harmony system.

  Remarks:
    This file requires access to the systemObjects global data structure that
    contains the object handles to all MPLAB Harmony module objects executing
    interrupt-driven in the system.  These handles are passed into the individual
    module "Tasks" functions to identify the instance of the module to maintain.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2011-2014 released Microchip Technology Inc.  All rights reserved.

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

#include "system/common/sys_common.h"
#include "app.h"
#include "system_definitions.h"

// *****************************************************************************
// *****************************************************************************
// Section: System Interrupt Vector Functions
// *****************************************************************************
// *****************************************************************************
void __ISR(_UART_1_VECTOR, ipl4AUTO) _IntHandlerDrvUsartInstance0(void)
{
    int8_t c;    
    
    BSP_LEDToggle(BSP_LED_1);   //debug
    
    // Is this an Error interrupt ?
    if ( PLIB_INT_SourceFlagGet(INT_ID_0, INT_SOURCE_USART_1_ERROR) &&
                 PLIB_INT_SourceIsEnabled(INT_ID_0, INT_SOURCE_USART_1_ERROR) ) {
        /* Clear pending interrupt */
        PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_1_ERROR);
        // Traitement de l'erreur à la réception.
    }
     
    // Is this an RX interrupt ?
    // réception UART et copie des datas dans fifo soft
    if ( PLIB_INT_SourceFlagGet(INT_ID_0, INT_SOURCE_USART_1_RECEIVE) &&
                 PLIB_INT_SourceIsEnabled(INT_ID_0, INT_SOURCE_USART_1_RECEIVE) ) {

        // transfert dans le FIFO software de tous les caracteres reçus 
        // ..avec controle d'erreur pour chaque byte (flags bufferises)
        while (PLIB_USART_ReceiverDataIsAvailable(USART_ID_1)/* && GetWriteSpace(&fifoLcd)>=6*/) {

            /* traitement des erreurs par caractere avant lecture */
            if (PLIB_USART_ExistsReceiverParityErrorStatus(USART_ID_1)) {
                if (PLIB_USART_ReceiverParityErrorHasOccurred(USART_ID_1)){
                    // remplir ici si traitement specifique des erreurs de parite
                }
            }
            if (PLIB_USART_ExistsReceiverFramingErrorStatus(USART_ID_1)) {
                if (PLIB_USART_ReceiverFramingErrorHasOccurred(USART_ID_1)) {
                    // remplir ici si traitement specifique des erreurs de framing
                }
            }
            
            /* lecture d'un caractere dans le buffer de l'UART */
            c = PLIB_USART_ReceiverByteReceive(USART_ID_1);
            
            //si le fifo est déjà plein, on laisse tomber le caractère à transmettre !
            // mieux vaut ça que de transmettre nimporte quoi !
            if (GetWriteSpace(&fifoLcd)>=6) 
                APP_SendCharToApp(c);
        }
        /* traitement des erreurs d'overrun */
        if (PLIB_USART_ExistsReceiverOverrunStatus(USART_ID_1)) {
            if (PLIB_USART_ReceiverOverrunHasOccurred(USART_ID_1)) {
                // En clearant, on efface tout le buffer de reception
                PLIB_USART_ReceiverOverrunErrorClear(USART_ID_1); 
                // remplir ici si traiement specifique
            }
        }
        /* quittancer l'interruption */
        PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_1_RECEIVE);
       
    } // end if RX
   
    // Is this an TX interrupt ?
    if ( PLIB_INT_SourceFlagGet(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT) &&
                 PLIB_INT_SourceIsEnabled(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT) ) {
		
        // disable TX interrupt (pour éviter une interrupt. inutile si plus rien à transmettre)
        PLIB_INT_SourceDisable(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT);
        
        // Clear the TX interrupt Flag (Seulement apres TX) 
        PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT);
    }    
}
 
//timer 1 @ 10Hz
void __ISR(_TIMER_1_VECTOR, ipl3AUTO) IntHandlerDrvTmrInstance0(void)
{
    int16_t iTemp;
    float fTemp;
    
    BSP_LEDToggle(BSP_LED_2);   //debug
    
    PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_TIMER_1);
    
    // lecture température 
    iTemp = SPI_ReadRawTempLM70();            
    LM70_ConvRawToDeg(iTemp, &fTemp);
    
    APP_SendTempToApp(fTemp);
}
 /*******************************************************************************
 End of File
*/
