#include <stdio.h>

#define nColumns	8						// Nc - could be 4, 6, or 8 columns for {128, 192, 256 bits}
#define nKeys		nColumns				// Nk = Nc
#define nRounds		( nColumns + 6 )		// Nn - when key and block sizes are both 128 bits, i.e. 40 bytes
											// nRounds could be 10, 12, or 14
#define nRows		4						// Nr - (always 4), i.e. 128 bit blocks require 4 columns with 32 bits each
#define AES_BLOCK_SIZE	( nColumns * nRows )

void	KeyExpansion( unsigned char* cipherKey, unsigned long* keySchedule );

void	cfb_encrypt(
	unsigned char *			iv,
	unsigned char *			ibuf,
	unsigned long			iBytes,
	unsigned char *			obuf,
	const unsigned long *	keySchedule );

/*
		// must have the following code before calling cfb_decrypt(), e.g. ...
	unsigned long iv[ nColumns ];			// the first <nColumns> words of PNMX_BOM.dat
	unsigned long key[ nColumns ];			// the last <nColumns> words of PNMX_BOM.dat
	FILE* fpIn = NULL;
		// if the BOM file size is greater than 4096, bail out - something is wrong
	errno_t err = fopen_s( &fpIn, "PNMX_BOM.dat", "rb" );


		// generate the key schedule
	unsigned long KeySchedule[ nKeys * (nRounds+1) ];			// constants defined in mrl_aescrypt.h
	KeyExpansion( (unsigned char*)key, KeySchedule );
	FILE* fpIn = NULL;
	errno_t err = fopen_s( &fpIn, "PNMX_BOM.dat", "rb" );

		// allocate a buffer for the BOM portion of the file ( filesize - 512 )
	size_t bytesRead = fread( iv, sizeof(char), AES_BLOCK_SIZE, fpIn );

	// seek to the end

	for ( ; ; )
	{	size_t bytesRead = fread( ibuf, sizeof(char), AES_BLOCK_SIZE, fpIn );
		if ( bytesRead <= 0 )
			break;
		short sres = cfb_encrypt( iv, ibuf, bytesRead, obuf, KeySchedule );
	}
*/


