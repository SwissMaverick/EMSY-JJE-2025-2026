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
#include "apptemp.h"
#include "applcd.h"
#include "system_definitions.h"
#include "semphr.h"
#include "queue.h"
#include "system_config.h"
#include "FreeRTOSConfig.h"



extern QueueHandle_t xQueueMessages;
extern SemaphoreHandle_t xSemaphoreTemperature;

// *****************************************************************************
// *****************************************************************************
// Section: System Interrupt Vector Functions
// *****************************************************************************
// *****************************************************************************
void IntHandlerDrvUsartInstance0(void)
{
    BSP_LEDToggle(BSP_LED_2);
    
    /*
    DRV_USART_TasksTransmit(sysObj.drvUsart0);
    DRV_USART_TasksError(sysObj.drvUsart0);
    DRV_USART_TasksReceive(sysObj.drvUsart0);
    */
    // Vérifier si un caractère est réellement disponible dans le buffer
    if (PLIB_USART_ReceiverDataIsAvailable(USART_ID_1))
    {
        // Lire le caractère
        char c = PLIB_USART_ReceiverByteReceive(USART_ID_1);

        // Préparer le message
        AppMessage_t msgToSend;
        msgToSend.id = MSG_TYPE_UART;
        msgToSend.data.caractere = c;

        BaseType_t xHigherPriorityTaskWoken = pdFALSE;

        // Envoi dans la queue depuis le contexte d'interruption
        if(xQueueMessages != NULL)
        {
            xQueueSendFromISR(xQueueMessages, &msgToSend, &xHigherPriorityTaskWoken);
        }

        // Forcer un changement de contexte
        portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
    }

    // Effacer les drapeaux d'interruption UART1 pour ne pas geler le système
    PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_1_RECEIVE);
    PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT);
    PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_1_ERROR);
}

void IntHandlerDrvTmrInstance0(void)
{
    BSP_LEDToggle(BSP_LED_3);
    
    PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_TIMER_2);
    // Variable requise par FreeRTOS pour savoir si une tâche plus prioritaire
    // doit s'exécuter immédiatement après l'interruption.
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // Libération du sémaphore depuis une ISR (Interrupt Service Routine)
    // Remplacez 'xSemaphoreTemperature' par le vrai nom de votre sémaphore.
    if(xSemaphoreTemperature != NULL)
    {
        xSemaphoreGiveFromISR(xSemaphoreTemperature, &xHigherPriorityTaskWoken);
    }

    // Forcer un changement de contexte si le sémaphore a réveillé
    // la Tâche 1 (qui est en attente de ce sémaphore).
    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
    
}
 /*******************************************************************************
 End of File
*/
