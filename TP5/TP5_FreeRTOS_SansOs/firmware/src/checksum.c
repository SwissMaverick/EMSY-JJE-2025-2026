// checksum.c
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

#include "checksum.h"

//donne la représentation ascii hex d'un nombre binaire
// binNr : nombre [0..15]
int8_t BinToAsciiHex (uint8_t binNr)
{
    if (binNr < 10) //chiffre 0..9 ?
        return (binNr + 48);
    else    //lettre A..F 
        return (binNr + 65 - 10);    
}


/*--------------------------------------------------------*/
//calcul le checksum 8 bits d'un tableau de bytes dataStart (longueur dataSize)
//le résultat est stocké en ascii hex. dans 2 caractères pointés par chksumHex
void ComputeChecksum(int8_t* dataStart, uint8_t dataSize, int8_t* chksumHex)
{
    uint8_t i;
    uint8_t chksum = 0;
     
    //calcule checksum
    for(i=0 ; i<dataSize ; i++)
        chksum += dataStart[i];
    
    chksumHex[0] = BinToAsciiHex(chksum >> 4);
    chksumHex[1] = BinToAsciiHex(chksum & 0x0F);    
}

/*--------------------------------------------------------*/

bool CheckChecksum(int8_t* dataStart, uint8_t dataSize, int8_t* chksumHex)
{
    int8_t computedChecksum[2];
    
    ComputeChecksum(dataStart, dataSize, computedChecksum); //calcule checksum du message
    
    return (computedChecksum[0]==chksumHex[0] && computedChecksum[1]==chksumHex[1]); //vérifie avec celle donnée        
}


/* *****************************************************************************
 End of File
 */
