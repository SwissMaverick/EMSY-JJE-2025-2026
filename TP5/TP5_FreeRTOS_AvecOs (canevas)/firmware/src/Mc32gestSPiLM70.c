/*--------------------------------------------------------*/
// Mc32GestSpiLM70.c
/*--------------------------------------------------------*/
//	Description :	Gestion par SPI du capteur temperature LM70 du Kit
//
//	Auteur 		: 	C. Huber
//	Version		:	V1.6    24.05.2016
//	Compilateur	:	XC32 V1.40 + Harmony 1.06
//      Modifications   :
//          CHR 06.03.2015 : Adaptation a plib_spi
//                           Utilisation stdint.h
//                           Ajout fonction SPI_CfgReadRawTempLM70
//          CHR 24.05.2016 : maj version compilateur et Harmony
//          CHR 26.05.2016  utilisation de SYS_CLK_PeripheralFrequencyGet(CLK_BUS_PERIPHERAL_1)
/*--------------------------------------------------------*/



//#include "app.h"   // pour system_config et autre
#include "Mc32gestSpiLM70.h"
#include "Mc32SpiUtil.h"
#include "peripheral\SPI\plib_spi.h"
#include "system/clk/sys_clk.h" // pour appels aux fonctions SYS_CLK_...())

// SPI_ID_1 correspond au SPI 1 !
#define KitSpi1 (SPI_ID_1)

uint32_t ConfigReg;     // pour lecture de SPI1CON
uint32_t BaudReg;       // pour lecture de SPI1BRG

// Definitions du bus
// #define SPI_SCL      PORTDbits.RD10  pin 70
// #define SPI_SDI      PORTCbits.RC4   pin 9
// #define SPI_SDO      PORTDbits.RD0   pin 72
// #define CS_LM70      LATDbits.LATD3  pin 78

// Initialisation de la communication SPI pour le LM70
// ---------------------------------------------------

void SPI_ConfigureLM70(void)
{
   PLIB_SPI_Disable(KitSpi1);

   PLIB_SPI_BufferClear(KitSpi1);
   PLIB_SPI_StopInIdleDisable(KitSpi1);
   PLIB_SPI_PinEnable(KitSpi1, SPI_PIN_DATA_OUT);
   PLIB_SPI_CommunicationWidthSelect(KitSpi1, SPI_COMMUNICATION_WIDTH_8BITS);
   // LM70 MAX 6.25 MHz choix 5 MHz
   PLIB_SPI_BaudRateSet(KitSpi1, SYS_CLK_PeripheralFrequencyGet(CLK_BUS_PERIPHERAL_1), 5000000);
   // Config polarité traitement des signaux SPI
   // pour input à confirmer
   // Polarité clock OK
   // Phase output à confirmer
   PLIB_SPI_InputSamplePhaseSelect(KitSpi1, SPI_INPUT_SAMPLING_PHASE_IN_MIDDLE );
   PLIB_SPI_ClockPolaritySelect(KitSpi1, SPI_CLOCK_POLARITY_IDLE_LOW);
   PLIB_SPI_OutputDataPhaseSelect(KitSpi1, SPI_OUTPUT_DATA_PHASE_ON_IDLE_TO_ACTIVE_CLOCK);
   PLIB_SPI_MasterEnable(KitSpi1);
   PLIB_SPI_FramedCommunicationDisable(KitSpi1);
   PLIB_SPI_FIFOEnable(KitSpi1);     // Enhenced buffer mode

   PLIB_SPI_Enable(KitSpi1);

   // Contrôle de la configuration
   ConfigReg = SPI1CON;
   BaudReg = SPI1BRG;
}


void SPI_InitLM70(void)  {

   
   SPI_ConfigureLM70();
    
   // action de configuration
   CS_LM70 = 0;
   spi_read1(0xFF);
   spi_read1(0xFF);
   spi_read1(0);  // pour écrire 0
   spi_read1(0);  // pour écrire 0

   //Fin de transmission
   CS_LM70 = 1;
  
} // SPI_InitLM70



// Lecture du registre de temperature du LM70
// Version sans reconfiguration
int16_t SPI_ReadRawTempLM70(void)
{
   // Déclaration des variables
   uint8_t MSB;
   uint8_t LSB;
   int16_t RawTemp;
     
   CS_LM70 = 0;
   MSB = spi_read1(0xFF);
   LSB = spi_read1(0xFF);
   //Fin de transmission
   CS_LM70 = 1;

   RawTemp = MSB;
   RawTemp = RawTemp << 8;
   RawTemp = RawTemp | LSB;
   return RawTemp;
} // SPI_ReadRawTempLM70

// Lecture du registre de temperature du LM70
// Version avec reconfiguration
int16_t SPI_CfgReadRawTempLM70(void)
{

   //Déclaration des variables
   uint8_t MSB;
   uint8_t LSB;
   int16_t RawTemp;

   SPI_ConfigureLM70();

   
   CS_LM70 = 0;
   MSB = spi_read1(0xFF);
   LSB = spi_read1(0xFF);
   //Fin de transmission
   CS_LM70 = 1;

   RawTemp = MSB;
   RawTemp = RawTemp << 8;
   RawTemp = RawTemp | LSB;
   return RawTemp;
} // SPI_CfgReadRawTempLM70
   


void LM70_ConvRawToDeg( int16_t RawTemp, float *PTemp)
{
    float TempLoc;

    RawTemp = RawTemp / 32;  // par division pour assurer le signe
    // Bit poid faible = 0.25 degré
    TempLoc = RawTemp * 0.25;
    *PTemp = TempLoc;
}

 



