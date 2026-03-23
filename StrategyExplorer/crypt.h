#include <stdio.h>

// Rijndael stuff
#define nRjRows				4									// Nr - (always 4), i.e. 128 bit blocks require 4 columns with 32 bits each
#define nRjColumns			8									// Nc - could be 4, 6, or 8 columns for {128, 192, 256 bits}
#define AES_BLOCK_SIZE		( nRjColumns * nRjRows )
#define nRjKeys				nRjColumns							// Nk = Nc
#define nRjRounds			( nRjColumns + 6 )					// Nn - when key and block sizes are both 128 bits, i.e. 40 bytes
																// nRounds could be 10, 12, or 14
#define nRjRoundKeys		( nRjColumns * ( nRjRounds + 1 ) )	// i.e. 120

void	rjKeySpan( unsigned char* cipherKey, unsigned long* keySchedule );		// Rijndael Key Expansion
void	rjDecrypt(												// we'll initialize iv within 
	unsigned char *			iv,
	unsigned char *			ibuf,
	unsigned long			iBytes,
	unsigned char *			obuf,
	const unsigned long *	roundKeys );
//-----------------------------------------------------------------------------
// RC6 stuff
/*
#define nBitsPerWord		32									// w
#define nRcRounds			20									// r
#define nRcBitsInKey		256									// b
#define LGw					5									// base two logarithm of w (nBitsPerWord)
#define nRcRoundKeys		( 2 * ( nRcRounds + 2 ) )			// 44
#define nRcWordsInKey		( nRcBitsInKey / nBitsPerWord )		// c - i.e. 8 words
#define nRcBytesInKey		( nRcWordsInKey * 4 )
#define nRcWordsPerBlock	4									// like nColumns in Rijndael ?
#define RC6_BLOCK_SIZE		( nRcWordsPerBlock * 4 )			// number of bytes per block * 4 bytes per word

void	rcKeySpan( unsigned long* cipherKey, unsigned long* keySchedule );		// RC6 Key Expansion
void	rcCrypt(
	unsigned char *			iv,
	unsigned char *			ibuf,
	unsigned long			iBytes,
	unsigned char *			obuf,
	const unsigned long	*	roundKeys );
*/
//-----------------------------------------------------------------------------
//unsigned int RotateLeft( unsigned int arg, unsigned int nBits );
//short	EEcode( char* ibuf, unsigned long* ckiv, char*& mimebuf, DWORD& mimelen );		// i.e. EncryptEncode

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
		cfb_encrypt( iv, ibuf, bytesRead, obuf, KeySchedule );
	}
*/


