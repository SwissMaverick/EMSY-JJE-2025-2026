#ifndef Mc32GestSpiLM70_H
#define Mc32GestSpiLM70_H
/*--------------------------------------------------------*/
// Mc32GestSpiLM70.h
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
//          CHR 24.05.2016  maj version compilateur et Harmony 
//
/*--------------------------------------------------------*/



#include <stdint.h>

// Prototypes des fonctions
void SPI_InitLM70(void);
int16_t SPI_ReadRawTempLM70(void);
// Nouveau avec reconfiguration
int16_t SPI_CfgReadRawTempLM70(void);

void LM70_ConvRawToDeg( int16_t RowTemp, float *PTemp);

#endif
