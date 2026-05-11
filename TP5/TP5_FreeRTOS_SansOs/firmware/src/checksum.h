// checksum.h
//
//	Description : 	fonctions de calcul et vérification de checksum 8 bits 
//                  codé en 2 digits hexadécimaux ASCII 
//	Auteur 		: 	SCA
//  Création	: 	08.04.2014
//
//  LISTE DES MODIFICATIONS :
//     -
//
//  Version KIT     PCB 11020D
//	Version		:	V1.0
//	Compilateur	:	XC32 V2.15 + Harmony 2.06
//
/*--------------------------------------------------------*/

#ifndef CHECKSUM_H
#define	CHECKSUM_H

#include <stdint.h>
#include <stdbool.h>


void ComputeChecksum(int8_t* dataStart, uint8_t dataSize, int8_t* chksumHex);

bool CheckChecksum(int8_t* dataStart, uint8_t dataSize, int8_t* chksumHex);

#endif	/* CHECKSUM_H */

