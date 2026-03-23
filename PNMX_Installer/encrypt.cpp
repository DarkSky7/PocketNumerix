// mrl_aescrypt.cpp
// refer to Brian Gladman's April 2003 v3.6 paper on Rijndael
#include "StdAfx.h"
#include "decrypt.h"
#include <assert.h>
#include <memory.h>
#include <io.h>				// _filelength

#define nCells		( nRows * nColumns )	// the AES_BLOCK_SIZE in bytes

#define lp32(x)			((unsigned int*)(x))
#define aligned_04(x)	(((unsigned int)(x) & 3) == 0)

const unsigned char Sbox[256] =
{
	 99,	124,	119,	123,	242,	107,	111,	197,	 48,	  1,	103,	 43,	254,	215,	171,	118,
	202,	130,	201,	125,	250,	 89,	 71,	240,	173,	212,	162,	175,	156,	164,	114,	192,
	183,	253,	147,	 38,	 54,	 63,	247,	204,	 52,	165,	229,	241,	113,	216,	 49,	 21,
	  4,	199,	 35,	195,	 24,	150,	  5,	154,	  7,	 18,	128,	226,	235,	 39,	178,	117,
	  9,	131,	 44,	 26,	 27,	110,	 90,	160,	 82,	 59,	214,	179,	 41,	227,	 47,	132,
	 83,	209,	  0,	237,	 32,	252,	177,	 91,	106,	203,	190,	 57,	 74,	 76,	 88,	207,
	208,	239,	170,	251,	 67,	 77,	 51,	133,	 69,	249,	  2,	127,	 80,	 60,	159,	168,
	 81,	163,	 64,	143,	146,	157,	 56,	245,	188,	182,	218,	 33,	 16,	255,	243,	210,
	205,	 12,	 19,	236,	 95,	151,	 68,	 23,	196,	167,	126,	 61,	100,	 93,	 25,	115,
	 96,	129,	 79,	220,	 34,	 42,	144,	136,	 70,	238,	184,	 20,	222,	 94,	 11,	219,
	224,	 50,	 58,	 10,	 73,	  6,	 36,	 92,	194,	211,	172,	 98,	145,	149,	228,	121,
	231,	200,	 55,	109,	141,	213,	 78,	169,	108,	 86,	244,	234,	101,	122,	174,	  8,
	186,	120,	 37,	 46,	 28,	166,	180,	198,	232,	221,	116,	 31,	 75,	189,	139,	138,
	112,	 62,	181,	102,	 72,	  3,	246,	 14,	 97,	 53,	 87,	185,	134,	193,	 29,	158,
	225,	248,	152,	 17,	105,	217,	142,	148,	155,	 30,	135,	233,	206,	 85,	 40,	223,
	140,	161,	137,	 13,	191,	230,	 66,	104,	 65,	153,	 45,	 15,	176,	 84,	187,	 22
};
/*
unsigned char GF256mul( unsigned char aa, unsigned char bb )
{		// Multiply two numbers in the GF(2^8) finite field defined 
		// by  Rijndael's irreducible polynomial:  x^8 + x^4 + x^3 + x + 1
		// refer to:  http://en.wikipedia.org/wiki/Finite_field_arithmetic
		//			  http://www.cs.utsa.edu/~wagner/laws/FFM.html
		//	and		  Brian Gladman's April 2003 v3.6 paper on Rijndael
	unsigned char	pp = 0;
	unsigned char	bit;
	unsigned char	hi_bit_set;
	for ( bit = 0; bit < 8; bit++ )
	{	if ( (bb & 1) == 1 )
			pp ^= aa;
		hi_bit_set = (aa & 0x80);
		aa <<= 1;
		if ( hi_bit_set != 0 )
			aa ^= 0x1b;				// x^8 + x^4 + x^3 + x + 1 
		bb >>= 1;
    }
    return	pp;
}			// GF256mul()
*/
//------------------------------------------------------------------------------------------------
inline unsigned char	GFmul( unsigned char aa, unsigned char bb )
{		// the logarithms (L) table from http://www.cs.utsa.edu/~wagner/laws/FFM.html
	const static unsigned char Log[ 256 ] =
	{		// the 0,0 entry is irrelevant (no log of 0), but the code works around it
		0x00, 0x00, 0x19, 0x01, 0x32, 0x02, 0x1a, 0xc6, 0x4b, 0xc7, 0x1b, 0x68, 0x33, 0xee, 0xdf, 0x03, 
		0x64, 0x04, 0xe0, 0x0e, 0x34, 0x8d, 0x81, 0xef, 0x4c, 0x71, 0x08, 0xc8, 0xf8, 0x69, 0x1c, 0xc1,
		0x7d, 0xc2, 0x1d, 0xb5, 0xf9, 0xb9, 0x27, 0x6a, 0x4d, 0xe4, 0xa6, 0x72, 0x9a, 0xc9, 0x09, 0x78,
		0x65, 0x2f, 0x8a, 0x05, 0x21, 0x0f, 0xe1, 0x24, 0x12, 0xf0, 0x82, 0x45, 0x35, 0x93, 0xda, 0x8e,
		0x96, 0x8f, 0xdb, 0xbd, 0x36, 0xd0, 0xce, 0x94, 0x13, 0x5c, 0xd2, 0xf1, 0x40, 0x46, 0x83, 0x38,
		0x66, 0xdd, 0xfd, 0x30, 0xbf, 0x06, 0x8b, 0x62, 0xb3, 0x25, 0xe2, 0x98, 0x22, 0x88, 0x91, 0x10,
		0x7e, 0x6e, 0x48, 0xc3, 0xa3, 0xb6, 0x1e, 0x42, 0x3a, 0x6b, 0x28, 0x54, 0xfa, 0x85, 0x3d, 0xba,
		0x2b, 0x79, 0x0a, 0x15, 0x9b, 0x9f, 0x5e, 0xca, 0x4e, 0xd4, 0xac, 0xe5, 0xf3, 0x73, 0xa7, 0x57,
		0xaf, 0x58, 0xa8, 0x50, 0xf4, 0xea, 0xd6, 0x74, 0x4f, 0xae, 0xe9, 0xd5, 0xe7, 0xe6, 0xad, 0xe8,
		0x2c, 0xd7, 0x75, 0x7a, 0xeb, 0x16, 0x0b, 0xf5, 0x59, 0xcb, 0x5f, 0xb0, 0x9c, 0xa9, 0x51, 0xa0,
		0x7f, 0x0c, 0xf6, 0x6f, 0x17, 0xc4, 0x49, 0xec, 0xd8, 0x43, 0x1f, 0x2d, 0xa4, 0x76, 0x7b, 0xb7,
		0xcc, 0xbb, 0x3e, 0x5a, 0xfb, 0x60, 0xb1, 0x86, 0x3b, 0x52, 0xa1, 0x6c, 0xaa, 0x55, 0x29, 0x9d,
		0x97, 0xb2, 0x87, 0x90, 0x61, 0xbe, 0xdc, 0xfc, 0xbc, 0x95, 0xcf, 0xcd, 0x37, 0x3f, 0x5b, 0xd1,
		0x53, 0x39, 0x84, 0x3c, 0x41, 0xa2, 0x6d, 0x47, 0x14, 0x2a, 0x9e, 0x5d, 0x56, 0xf2, 0xd3, 0xab,
		0x44, 0x11, 0x92, 0xd9, 0x23, 0x20, 0x2e, 0x89, 0xb4, 0x7c, 0xb8, 0x26, 0x77, 0x99, 0xe3, 0xa5,
		0x67, 0x4a, 0xed, 0xde, 0xc5, 0x31, 0xfe, 0x18, 0x0d, 0x63, 0x8c, 0x80, 0xc0, 0xf7, 0x70, 0x07
	};
		// the exponentials (E) table from http://www.cs.utsa.edu/~wagner/laws/FFM.html
	const static unsigned char Antilog[ 256 ] = 
	{	0x01, 0x03, 0x05, 0x0f, 0x11, 0x33, 0x55, 0xff, 0x1a, 0x2e, 0x72, 0x96, 0xa1, 0xf8, 0x13, 0x35, 
		0x5f, 0xe1, 0x38, 0x48, 0xd8, 0x73, 0x95, 0xa4, 0xf7, 0x02, 0x06, 0x0a, 0x1e, 0x22, 0x66, 0xaa,
		0xe5, 0x34, 0x5c, 0xe4, 0x37, 0x59, 0xeb, 0x26, 0x6a, 0xbe, 0xd9, 0x70, 0x90, 0xab, 0xe6, 0x31,
		0x53, 0xf5, 0x04, 0x0c, 0x14, 0x3c, 0x44, 0xcc, 0x4f, 0xd1, 0x68, 0xb8, 0xd3, 0x6e, 0xb2, 0xcd,
		0x4c, 0xd4, 0x67, 0xa9, 0xe0, 0x3b, 0x4d, 0xd7, 0x62, 0xa6, 0xf1, 0x08, 0x18, 0x28, 0x78, 0x88,
		0x83, 0x9e, 0xb9, 0xd0, 0x6b, 0xbd, 0xdc, 0x7f, 0x81, 0x98, 0xb3, 0xce, 0x49, 0xdb, 0x76, 0x9a,
		0xb5, 0xc4, 0x57, 0xf9, 0x10, 0x30, 0x50, 0xf0, 0x0b, 0x1d, 0x27, 0x69, 0xbb, 0xd6, 0x61, 0xa3,
		0xfe, 0x19, 0x2b, 0x7d, 0x87, 0x92, 0xad, 0xec, 0x2f, 0x71, 0x93, 0xae, 0xe9, 0x20, 0x60, 0xa0,
		0xfb, 0x16, 0x3a, 0x4e, 0xd2, 0x6d, 0xb7, 0xc2, 0x5d, 0xe7, 0x32, 0x56, 0xfa, 0x15, 0x3f, 0x41,
		0xc3, 0x5e, 0xe2, 0x3d, 0x47, 0xc9, 0x40, 0xc0, 0x5b, 0xed, 0x2c, 0x74, 0x9c, 0xbf, 0xda, 0x75,
		0x9f, 0xba, 0xd5, 0x64, 0xac, 0xef, 0x2a, 0x7e, 0x82, 0x9d, 0xbc, 0xdf, 0x7a, 0x8e, 0x89, 0x80,
		0x9b, 0xb6, 0xc1, 0x58, 0xe8, 0x23, 0x65, 0xaf, 0xea, 0x25, 0x6f, 0xb1, 0xc8, 0x43, 0xc5, 0x54,
		0xfc, 0x1f, 0x21, 0x63, 0xa5, 0xf4, 0x07, 0x09, 0x1b, 0x2d, 0x77, 0x99, 0xb0, 0xcb, 0x46, 0xca,
		0x45, 0xcf, 0x4a, 0xde, 0x79, 0x8b, 0x86, 0x91, 0xa8, 0xe3, 0x3e, 0x42, 0xc6, 0x51, 0xf3, 0x0e,
		0x12, 0x36, 0x5a, 0xee, 0x29, 0x7b, 0x8d, 0x8c, 0x8f, 0x8a, 0x85, 0x94, 0xa7, 0xf2, 0x0d, 0x17,
		0x39, 0x4b, 0xdd, 0x7c, 0x84, 0x97, 0xa2, 0xfd, 0x1c, 0x24, 0x6c, 0xb4, 0xc7, 0x52, 0xf6, 0x01
	};
	if ( aa == 0  ||  bb == 0 )
		return	0;
	short	tt = *(Log+aa) + *(Log+bb);
	if ( tt > 255 )
		tt -= 255;
	return	*(Antilog+tt);
}			// GFmul()
//------------------------------------------------------------------------------------------------
inline unsigned int	SubWord( unsigned int xx )
{		// perform Sbox substitutions on an entire word
	unsigned char* cc = (unsigned char*)&xx;
	*(cc+0) = *(Sbox + *cc);
	*(cc+1) = *(Sbox + *cc);
	*(cc+2) = *(Sbox + *cc);
	*(cc+3) = *(Sbox + *cc);
	return	xx;
}			// SubWord()
//------------------------------------------------------------------------------------------------
inline unsigned int	RotateWord( unsigned int xx )
{		// circularly shift xx one byte to the right
	unsigned char cc = xx & 0xFF;				// save the low order byte
	xx = xx >> 8;									// downshift (right
	unsigned long ll = (unsigned long)cc;
	xx |= ( ll << 24 );						// put the saved byte (cc) back in at the top
	return	xx;
}			// RotateWord()
//------------------------------------------------------------------------------------------------
// refer to Brian Gladman's April 2003 v3.6 paper on Rijndael
// Brian Gladman shows construction of a word from bytes in the key
// the addresses of the key bytes are highest on the left, lowest on the right, thus little Endian
// so we can just copy the cipher key a word at a time.  we can simply address a word as 4 contiguous
// bytes
// with the same address as the lowest address byte, so his first while loop isn't necessary
void	KeyExpansion( unsigned char* cipherKey, unsigned long* keySchedule )
{		// input: cipherKey 
		// output: keySchedule
	for ( short ii = 0; ii < nKeys; ii++ )
	{		// copy cipherKeys into the first words of the key schedule
		*(keySchedule+ii) = *((unsigned long*)cipherKey+ii);
	}
		// According to:  http://www.quadibloc.com/crypto/co040401.htm, the first 36 round constants are:
		//  1   2   4   8  16  32  64 128  27  54 108 216 171  77 154  47  94 188
		// 99 198 151  53 106 212 179 125 250 239 197 145  57 114 228 211 189  97 ...
		// we only need the first 15 to cover 14 encryption rounds (need nRounds + 1)
#if _DEBUG
	assert( nRounds <= 14 );
#endif
	const unsigned char Rcon[15] = { 1, 2, 4, 8, 16, 32, 64, 128, 27, 54, 108, 216, 171, 77, 154 };

	short maxRoundKey = nColumns * (nRounds + 1);		// Nc(Nr+1):  4(10), 6(12), 8(14) -> 44, 78, 120
	for ( short ii = nKeys; ii < maxRoundKey; ii++ )
	{	unsigned int temp = *(keySchedule+ii);
		if ( (ii % nKeys) == 0 )
			temp = SubWord( RotateWord(temp) ) ^ *(Rcon+ ii/nKeys);
		else if ( nKeys > 6  &&  (ii % nKeys) == 4 )
			temp = SubWord( temp );
		*(keySchedule+ii) = *(keySchedule+ii-nKeys) ^ temp;
	}
}			// KeyExpansion()
//------------------------------------------------------------------------------------------------
inline void	SubBytes( unsigned char* state )
{	for ( short ii = 0; ii < nCells; ii++ )
		*(state+ii) = *(Sbox + *(state+ii));
}			// SubBytes()
//------------------------------------------------------------------------------------------------
#if ( nColumns == 4 )
void inline ShiftRows( unsigned char* state )
{		// use with 128 bit blocks
		// bytes are stored column first, row second, e.g.
		// before...	afterwards, the array looks like...
		//  0  4  8 12		 0  4  8 12		leave the first row alone
		//  1  5  9 13		 5  9 13  1		circularly 'rotate' one byte left
		//  2  6 10 14  -->	10 14  2  6		circularly 'rotate' by two bytes
		//  3  7 11 15		15  3  7 11		circularly 'rotate' by two bytes
		// refer to:  http://www.quadibloc.com/crypto/co040401.htm
																		//  0
	unsigned char s1  = *(state+ 1);	*(state+ 1) = *(state+ 5);		//  5
	unsigned char s2  = *(state+ 2);	*(state+ 2) = *(state+10);		// 10
	unsigned char s3  = *(state+ 3);	*(state+ 3) = *(state+15);		// 15
																		//  4
										*(state+ 5) = *(state+ 9);		//  9
	unsigned char s6  = *(state+ 6);	*(state+ 6) = *(state+14);		// 14
	unsigned char s7  = *(state+ 7);	*(state+ 7) = s3;				//  3
																		//  8
										*(state+ 9) = *(state+13);		// 13
										*(state+10) = s2;				//  2
	unsigned char s11 = *(state+11);	*(state+11) = s7;				//  7
																		// 12
										*(state+13) = s1;				//  1
										*(state+14) = s6;				//  6
										*(state+15) = s11;				// 11
}			// ShiftRows()
#endif
//------------------------------------------------------------------------------------------------
#if ( nColumns == 6 )
void inline ShiftRows( unsigned char* state )
{		// use with 192 bit blocks
		// bytes are stored column first, row second, e.g.
		// before...			afterwards, the array looks like...
		// 0  4  8 12 16 20			 0  4  8 12	16 20	leave the first row alone
		// 1  5  9 13 17 21			 5  9 13 17 21  1	circularly 'rotate' one byte left
		// 2  6 10 14 18 22  --->	10 14 18 22  2  6	circularly 'rotate' two bytes left
		// 3  7 11 15 19 23			15 19 23  3  7 11	circularly 'rotate' three bytes
		// refer to:  http://www.quadibloc.com/crypto/co040401.htm
																		//  0
	unsigned char s1  = *(state+ 1);	*(state+ 1) = *(state+ 5);		//  5
	unsigned char s2  = *(state+ 2);	*(state+ 2) = *(state+10);		// 10
	unsigned char s3  = *(state+ 3);	*(state+ 3) = *(state+15);		// 15
																		//  4
										*(state+ 5) = *(state+ 9);		//  9
	unsigned char s6  = *(state+ 6);	*(state+ 6) = *(state+14);		// 14
	unsigned char s7  = *(state+ 7);	*(state+ 7) = *(state+19);		// 19
																		//  8
										*(state+ 9) = *(state+13);		// 13
										*(state+10) = *(state+18);		// 18
	unsigned char s11 = *(state+11);	*(state+11) = *(state+23);		// 23
																		// 12
										*(state+13) = *(state+17);		// 17
										*(state+14) = *(state+22);		// 22
										*(state+15) = s3;				//  3
																		// 16
										*(state+17) = *(state+21);		// 21
										*(state+18) = s2;				//  2
										*(state+19) = s7;				//  7
																		// 20
										*(state+21) = s1;				//  1
										*(state+22) = s6;				//  6
										*(state+23) = s11;				// 23
}			// ShiftRows()
#endif
//------------------------------------------------------------------------------------------------
#if ( nColumns == 8 )
void inline ShiftRows( unsigned char* state )
{		// use with 256 bit blocks
		// bytes are stored column first, row second, e.g.
		// before...				afterwards, the array looks like...
		// 0  4  8 12 16 20 24 28		 0  4  8 12	16 20 24 28		leave the first row alone
		// 1  5  9 13 17 21	25 29		 5  9 13 17 21 25 29  1		circularly 'rotate' one byte left
		// 2  6 10 14 18 22 26 30  -->	14 18 22 26 30  2  6 10		circularly 'rotate' three bytes left
		// 3  7 11 15 19 23 27 31		19 23 27 31  3  7 11 15		circularly 'rotate' four bytes
		// refer to:  http://www.quadibloc.com/crypto/co040401.htm
																		//  0
	unsigned char s1  = *(state+ 1);	*(state+ 1) = *(state+ 5);		//  5
	unsigned char s2  = *(state+ 2);	*(state+ 2) = *(state+14);		// 14
	unsigned char s3  = *(state+ 3);	*(state+ 3) = *(state+19);		// 19
																		//  4
										*(state+ 5) = *(state+ 9);		//  9
	unsigned char s6  = *(state+ 6);	*(state+ 6) = *(state+18);		// 18
	unsigned char s7  = *(state+ 7);	*(state+ 7) = *(state+23);		// 23
																		//  8
										*(state+ 9) = *(state+13);		// 13
	unsigned char s10 = *(state+10);	*(state+10) = *(state+22);		// 22
	unsigned char s11 = *(state+11);	*(state+11) = *(state+27);		// 27
																		// 12
										*(state+13) = *(state+17);		// 17
										*(state+14) = *(state+26);		// 26
	unsigned char s15 = *(state+15);	*(state+15) = *(state+31);		// 31
																		// 16
										*(state+17) = *(state+21);		// 21
										*(state+18) = *(state+30);		// 30
										*(state+19) = s3;				//  3
																		// 20
										*(state+21) = *(state+25);		// 25
										*(state+22) = s2;				//  2
										*(state+23) = s7;				//  7
																		// 24
										*(state+25) = *(state+29);		// 29
										*(state+26) = s6;				//  6
										*(state+27) = s11;				// 11
																		// 28
										*(state+29) = s1;				//  1
										*(state+30) = s10;				// 10
										*(state+31) = s15;				// 15
}			// ShiftRows()
#endif
//------------------------------------------------------------------------------------------------
// see page 12 of Brian Gladman's Apr-03 v3.6 Specification for Rijndael, the AES Algorithm
// pp.11-12 of the following presentation is succinct:
// http://eee.ucc.ie/staff/marnanel/Files/handoutee5251/lec-AES2.pdf, but has a different matrix
// for for the forward cipher and no matrix for the inverse cipher.
//	A is a column of state; and C is the polynomial 03 x^3 + 01 x^2 + 01 x^1 + 02
// so (03 x^3 + 01 x^2 + 01 x^1 + 02) mod ( x^4 + 1 ) in a GF(2^8) Galois Field
// becomes the coefficient matrix below
// | b0 |	  | 02 01 01 03 | | a0 |
// | b1 |  =  | 03 02 01 01 | | a1 |
// | b2 |     | 01 03 02 01 | | a2 |
// | b3 |     | 01 01 03 02 | | a3 |
void	MixColumns( unsigned char* state )
{		// this routine depends on little endianess
		// make a copy of state
	for ( short cc = 0; cc < nColumns; cc++ )
	{	unsigned long* col = (unsigned long*)state + cc;		// alias for current state column
		unsigned char xx[4] = { *((unsigned char*)col+0),		// copy the column
								*((unsigned char*)col+1),		// because we're going to overwrite it
								*((unsigned char*)col+2),
								*((unsigned char*)col+3)	};
		*((unsigned char*)col+0) =								// multiply by | 02 03 01 01 |
			GFmul(2,xx[0]) ^ xx[1] ^ xx[2] ^ GFmul(3,xx[3]);	// and add (xor in a Galois Field)

		*((unsigned char*)col+1) =								// multiply by | 01 02 03 01 |
			GFmul(3,xx[0]) ^ GFmul(2,xx[1]) ^ xx[2] ^ xx[3];	// and add (xor in a Galois Field)

		*((unsigned char*)col+2) =								// multiply by | 01 01 02 03 |
			xx[0] ^ GFmul(3,xx[1]) ^ GFmul(2,xx[2]) ^ xx[3];	// and add (xor in a Galois Field)

		*((unsigned char*)col+3) =								// multiply by | 03 01 01 02 |
			xx[0] ^ xx[1] ^ GFmul(3,xx[2]) ^ GFmul(2,xx[3]);	// and add (xor in a Galois Field)
	}
}			// MixColumns()
//------------------------------------------------------------------------------------------------
// XOR each column of state with the keySchedule (a subset of the round key schedule)
// an example call looks like:
//		XorRoundKey( (unsigned long*)state, (unsigned long*)keySchedule+colNum );
// where rn is the round number
// XorRoundRey is its own inverse
void	inline XorRoundKey( unsigned long* state, const unsigned long* keySchedule )
{	for ( unsigned short cc = 0; cc < nColumns; cc++ )
		*(state+cc) ^= *(keySchedule+cc);
}			// XorRoundKey()
//------------------------------------------------------------------------------------------------
// requires generation of a keySchedule
void	Cipher( unsigned char* ibuf, unsigned char* obuf, const unsigned long* keySchedule )
{	unsigned char	state[ nCells ];
	memcpy( state, ibuf, nCells );

	XorRoundKey( (unsigned long*)state, keySchedule );
	unsigned short	unmixedRound = nRounds - 1;		// where nRounds = 10, 12, or 14
	for ( short rn = 1; rn < unmixedRound; rn++ )	// note that round 0 already happened
	{	SubBytes( state );
		ShiftRows( state );
		MixColumns( state );
		XorRoundKey( (unsigned long*)state, keySchedule+rn );
	}
	SubBytes( state );
	ShiftRows( state );
	XorRoundKey( (unsigned long*)state, keySchedule+unmixedRound );

	memcpy( obuf, state, nCells );
}			// Cipher()
//------------------------------------------------------------------------------------------------
void	cfb_encrypt(
	unsigned char *			iv,
	unsigned char *			ibuf,
	unsigned long			iBytes,
	unsigned char *			obuf,				// allocated in the caller
	const unsigned long	*	roundKeys )
{		// Refer to: http://en.wikipedia.org/wiki/Block_cipher_modes_of_operation
	while ( iBytes > 0 )
	{	Cipher( iv, iv, key );
		if ( bytesRead == AES_BLOCK_SIZE )
		{	if ( aligned_04(ibuf)  &&  aligned_04(obuf)  &&  aligned_04(iv) )
			{		// we always take this branch...
					// the following four statements are the unrolled version ...
				lp32(obuf)[0] = lp32(iv)[0] ^= lp32(ibuf)[0];
				lp32(obuf)[1] = lp32(iv)[1] ^= lp32(ibuf)[1];
				lp32(obuf)[2] = lp32(iv)[2] ^= lp32(ibuf)[2];
				lp32(obuf)[3] = lp32(iv)[3] ^= lp32(ibuf)[3];
#if ( nColumns > 4 )
				lp32(obuf)[4] = lp32(iv)[4] ^= lp32(ibuf)[4];
				lp32(obuf)[5] = lp32(iv)[5] ^= lp32(ibuf)[5];
#endif
#if ( nColumns > 6 )
				lp32(obuf)[6] = lp32(iv)[6] ^= lp32(ibuf)[6];
				lp32(obuf)[7] = lp32(iv)[7] ^= lp32(ibuf)[7];
#endif
			}
			else
			{		// the following sixteen statements are the unrolled version...
				obuf[ 0] = iv[ 0] ^= ibuf[ 0];	obuf[ 1] = iv[ 1] ^= ibuf[ 1];
				obuf[ 2] = iv[ 2] ^= ibuf[ 2];	obuf[ 3] = iv[ 3] ^= ibuf[ 3];
				obuf[ 4] = iv[ 4] ^= ibuf[ 4];	obuf[ 5] = iv[ 5] ^= ibuf[ 5];
				obuf[ 6] = iv[ 6] ^= ibuf[ 6];	obuf[ 7] = iv[ 7] ^= ibuf[ 7];
				obuf[ 8] = iv[ 8] ^= ibuf[ 8];	obuf[ 9] = iv[ 9] ^= ibuf[ 9];
				obuf[10] = iv[10] ^= ibuf[10];	obuf[11] = iv[11] ^= ibuf[11];
				obuf[12] = iv[12] ^= ibuf[12];	obuf[13] = iv[13] ^= ibuf[13];
				obuf[14] = iv[14] ^= ibuf[14];	obuf[15] = iv[15] ^= ibuf[15];
#if ( nColumns > 4 )
				obuf[16] = iv[16] ^= ibuf[16];	obuf[17] = iv[17] ^= ibuf[17];
				obuf[18] = iv[18] ^= ibuf[18];	obuf[19] = iv[19] ^= ibuf[19];
				obuf[20] = iv[20] ^= ibuf[20];	obuf[21] = iv[21] ^= ibuf[21];
				obuf[22] = iv[22] ^= ibuf[22];	obuf[23] = iv[23] ^= ibuf[23];
#endif
#if ( nColumns > 6 )
				obuf[24] = iv[24] ^= ibuf[24];	obuf[25] = iv[25] ^= ibuf[25];
				obuf[26] = iv[26] ^= ibuf[26];	obuf[27] = iv[27] ^= ibuf[27];
				obuf[28] = iv[28] ^= ibuf[28];	obuf[29] = iv[29] ^= ibuf[29];
				obuf[30] = iv[30] ^= ibuf[30];	obuf[31] = iv[31] ^= ibuf[31];
#endif
			}
		}
			iBytes -= AES_BLOCK_SIZE;
			obuf += AES_BLOCK_SIZE;
			ibuf += AES_BLOCK_SIZE;
		}
		else
		{	for ( short ii = 0; ii < iBytes; ii++ )
				*(obuf+ii) = *(ibuf+ii) ^ *(iv+ii);
			iBytes = 0;
		}
	}	// while ( oBytes < iBytes )
}			// cfb_encrypt()
//-----------------------------------------------------------------------
