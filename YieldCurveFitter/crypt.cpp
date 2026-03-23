// mrl_aescrypt.cpp
// refer to Brian Gladman's April 2003 v3.6 paper on Rijndael
#include "StdAfx.h"
#include "crypt.h"
#include "MimeEncode.h"			// for use by EncryptEncode()
#include <memory.h>
//#include <assert.h>
//#include <io.h>				// _filelength (no I/O in this version)
#ifdef ServerAccess
#define nRjCells		( nRjRows * nRjColumns )	// the AES_BLOCK_SIZE in bytes

#define lp32(x)			((unsigned int*)(x))
#define aligned_04(x)	(((unsigned int)(x) & 3) == 0)
#define DUMP_OBUF 0

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
	*(cc+0) = *(Sbox + *(cc+0));
	*(cc+1) = *(Sbox + *(cc+1));
	*(cc+2) = *(Sbox + *(cc+2));
	*(cc+3) = *(Sbox + *(cc+3));
	return	xx;
}			// SubWord()
//------------------------------------------------------------------------------------------------
inline unsigned int	RotateWord( unsigned int xx )
{		// circularly shift xx one byte to the right
	unsigned int cc = (xx & 0xFF) << 24;	// shift the low order byte into high position
	return	cc | (xx >> 8);					// combine with the rest, downshifted one byte
}			// RotateWord()
//------------------------------------------------------------------------------------------------
// refer to Brian Gladman's April 2003 v3.6 paper on Rijndael
// Brian Gladman shows construction of a word from bytes in the key
// the addresses of the key bytes are highest on the left, lowest on the right, thus little Endian
// so we can just copy the cipher key a word at a time.  we can simply address a word as 4 contiguous
// bytes
// with the same address as the lowest address byte, so his first while loop isn't necessary
void	rjKeySpan( unsigned char* ck, unsigned long* rk )
{		// input: cipherKey (ck)
		// output: keySchedule (rk)
	short	ii;
	for ( ii = 0; ii < nRjKeys; ii++ )				// nKeys = 8
	{		// copy cipherKeys into the first words of the key schedule
		*(rk+ii) = *((unsigned int*)ck+ii);
	}
		// According to:  http://www.quadibloc.com/crypto/co040401.htm, the first 36 round constants are:
		//  1   2   4   8  16  32  64 128  27  54 108 216 171  77 154  47  94 188
		// 99 198 151  53 106 212 179 125 250 239 197 145  57 114 228 211 189  97 ...
		// we only need the first 15 to cover 14 encryption rounds (need nRounds + 1)
#ifdef _DEBUG
//	assert( nRjRounds <= 14 );
#endif
	const unsigned char Rcon[15] = { 1, 2, 4, 8, 16, 32, 64, 128, 27, 54, 108, 216, 171, 77, 154 };
	short maxRoundKey = nRjKeys * (nRjRounds + 1);				// Nc(Nr+1):  4(10), 6(12), 8(14) -> 44, 78, 120
	for ( ii = nRjKeys; ii < maxRoundKey; ii++ )
	{	unsigned int temp = *(rk+ii-1);
		if ( (ii % nRjKeys) == 0 )
		{	unsigned long rconVal = *(Rcon + ii/nRjKeys - 1);
			unsigned long substWord = SubWord( RotateWord(temp) );
			temp = substWord ^ rconVal;		// the first usable ii == nKeys
		}
		else if ( nRjKeys > 6  &&  (ii % nRjKeys) == 4 )
			temp = SubWord( temp );
		*(rk+ii) = *(rk+ii-nRjKeys) ^ temp;
	}
}			// rjKeySpan()
//------------------------------------------------------------------------------------------------
inline void	SubBytes( unsigned char* state )
{	for ( short ii = 0; ii < nRjCells; ii++ )
		*(state+ii) = *(Sbox + *(state+ii));
}			// SubBytes()
//------------------------------------------------------------------------------------------------
#if ( nRjColumns == 4 )
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
#if ( nRjColumns == 6 )
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
#if ( nRjColumns == 8 )
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
	unsigned char xx[ nRjRows ];
	for ( short cc = 0; cc < nRjColumns; cc++ )
	{		// make a copy of the column
		for ( short rr = 0; rr < nRjRows; rr++ )				// we know nRows = 4 (always)
			*(xx+rr) = *(state + nRjColumns*rr + cc);
			// modify this column in the state
		*(state + nRjColumns*0 + cc) =							// multiply by | 02 01 01 03 |
			GFmul(2,xx[0]) ^ xx[1] ^ xx[2] ^ GFmul(3,xx[3]);	// and add (xor in a Galois Field)

		*(state + nRjColumns*1 + cc) =							// multiply by | 03 02 01 01 |
			GFmul(3,xx[0]) ^ GFmul(2,xx[1]) ^ xx[2] ^ xx[3];	// and add (xor in a Galois Field)

		*(state + nRjColumns*2 + cc) =							// multiply by | 01 03 02 01 |
			xx[0] ^ GFmul(3,xx[1]) ^ GFmul(2,xx[2]) ^ xx[3];	// and add (xor in a Galois Field)

		*(state + nRjColumns*3 + cc) =							// multiply by | 01 01 03 02 |
			xx[0] ^ xx[1] ^ GFmul(3,xx[2]) ^ GFmul(2,xx[3]);	// and add (xor in a Galois Field)
	}
}			// MixColumns()
//------------------------------------------------------------------------------------------------
// XOR each column of state with the keySchedule (a subset of the round key schedule)
// an example call looks like:
//		XorRoundKey( (unsigned int*)state, (unsigned int*)keySchedule+colNum );
// where rn is the round number
// XorRoundRey is its own inverse
void	inline XorRoundKey( unsigned long* state, const unsigned long* rk )
{	for ( unsigned short cc = 0; cc < nRjColumns; cc++ )
		*(state+cc) ^= *(rk+cc);
}			// XorRoundKey()
//------------------------------------------------------------------------------------------------
// requires generation of a keySchedule
void	rjCipher( unsigned char* ibuf, unsigned char* obuf, const unsigned long* rk )
{	unsigned char	state[ nRjCells ];
	memcpy( state, ibuf, nRjCells );
#define LOCAL_DEBUG		0
	XorRoundKey( (unsigned long*)state, rk );						// round 0 
#if LOCAL_DEBUG
	fprintf( stderr, "XorRoundRey(0) -->\r\n" );
	for ( short ii = 0; ii < AES_BLOCK_SIZE; ii++ )
		fprintf( stderr, " %02x", *(state+ii) );
#endif

	for ( short rn = 1; rn < nRjRounds; rn++ )						// round 1 to nRounds - 1
	{	SubBytes( state );
#if LOCAL_DEBUG
		fprintf( stderr, "\r\nSubBytes(%d) -->\r\n", rn );
		for ( short ii = 0; ii < AES_BLOCK_SIZE; ii++ )
			fprintf( stderr, " %02x", *(state+ii) );
#endif

		ShiftRows( state );
#if LOCAL_DEBUG
		fprintf( stderr, "\r\nShiftRows(%d) -->\r\n", rn );
		for ( short ii = 0; ii < AES_BLOCK_SIZE; ii++ )
			fprintf( stderr, " %02x", *(state+ii) );
#endif

		MixColumns( state );
#if LOCAL_DEBUG
		fprintf( stderr, "\r\nMixColumns(%d) -->\r\n", rn );
		for ( short ii = 0; ii < AES_BLOCK_SIZE; ii++ )
			fprintf( stderr, " %02x", *(state+ii) );
#endif

		XorRoundKey( (unsigned long*)state, rk + rn * nRjColumns );
#if LOCAL_DEBUG
		fprintf( stderr, "\r\nXorRoundKey(%d) -->\r\n", rn );
		for ( short ii = 0; ii < AES_BLOCK_SIZE; ii++ )
			fprintf( stderr, " %02x", *(state+ii) );
	fprintf( stderr, "\r\n - - - - - - - - - - - - - - - - - - - -" );
#endif
	}
	SubBytes( state );
#if LOCAL_DEBUG
	fprintf( stderr, "\r\nSubBytes(%d) -->\r\n", nRounds );
	for ( short ii = 0; ii < AES_BLOCK_SIZE; ii++ )
		fprintf( stderr, " %02x", *(state+ii) );
#endif

	ShiftRows( state );
#if LOCAL_DEBUG
	fprintf( stderr, "\r\nShiftRows(%d) -->\r\n", nRounds );
	for ( short ii = 0; ii < AES_BLOCK_SIZE; ii++ )
		fprintf( stderr, " %02x", *(state+ii) );
#endif

		// nRounds, i.e. 6, 10, or 14 (zero based) round 
	XorRoundKey( (unsigned long*)state, rk + nRjRounds * nRjColumns );
#if LOCAL_DEBUG
	fprintf( stderr, "\r\nXorRoundKey(%d) -->\r\n", nRounds );
	for ( short ii = 0; ii < AES_BLOCK_SIZE; ii++ )
		fprintf( stderr, " %02x", *(state+ii) );
	fprintf( stderr, "\r\n = = = = = = = = = = = = = = = =\r\n" );
#endif

	memcpy( obuf, state, nRjCells );
}			// rjCipher()
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
void	rjDecrypt(
	unsigned char *			iv,
	unsigned char *			ibuf,
	unsigned long			iBytes,
	unsigned char *			obuf,				// allocated in the caller
	const unsigned long	*	roundKeys )
{		// Refer to: http://en.wikipedia.org/wiki/Block_cipher_modes_of_operation
//	unsigned char	iv[ AES_BLOCK_SIZE ];		// these two lines will be needed in the caller...
//	memcpy( iv, initIv, AES_BLOCK_SIZE );		// initialize iv fresh
		// note that values within iv are modified here,
		// so the iv state can be passed to/from the caller 
	while ( iBytes > 0 )
	{	rjCipher( iv, iv, roundKeys );
		if ( iBytes >= AES_BLOCK_SIZE )
		{	if ( aligned_04(ibuf)  &&  aligned_04(obuf)  &&  aligned_04(iv) )
			{	unsigned int tt;			// we ALWAYS take this branch
				tt = lp32(ibuf)[0];  lp32(obuf)[0] = tt ^ lp32(iv)[0];	lp32(iv)[0] = tt;
				tt = lp32(ibuf)[1];  lp32(obuf)[1] = tt ^ lp32(iv)[1];	lp32(iv)[1] = tt;
				tt = lp32(ibuf)[2];  lp32(obuf)[2] = tt ^ lp32(iv)[2];	lp32(iv)[2] = tt;
				tt = lp32(ibuf)[3];	 lp32(obuf)[3] = tt ^ lp32(iv)[3];	lp32(iv)[3] = tt;
#if ( nRjColumns > 4 )
				tt = lp32(ibuf)[4];  lp32(obuf)[4] = tt ^ lp32(iv)[4];	lp32(iv)[4] = tt;
				tt = lp32(ibuf)[5];	 lp32(obuf)[5] = tt ^ lp32(iv)[5];	lp32(iv)[5] = tt;
#endif
#if ( nRjColumns > 6 )
				tt = lp32(ibuf)[6];  lp32(obuf)[6] = tt ^ lp32(iv)[6];	lp32(iv)[6] = tt;
				tt = lp32(ibuf)[7];	 lp32(obuf)[7] = tt ^ lp32(iv)[7];	lp32(iv)[7] = tt;
#endif
			}
			else
			{	unsigned char tt;
				tt = ibuf[ 0];	obuf[ 0] = tt ^ iv[ 0];  iv[ 0] = tt;
				tt = ibuf[ 1];	obuf[ 1] = tt ^ iv[ 1];  iv[ 1] = tt;
				tt = ibuf[ 2];	obuf[ 2] = tt ^ iv[ 2];  iv[ 2] = tt;
				tt = ibuf[ 3];	obuf[ 3] = tt ^ iv[ 3];  iv[ 3] = tt;
				tt = ibuf[ 4];	obuf[ 4] = tt ^ iv[ 4];  iv[ 4] = tt;
				tt = ibuf[ 5];	obuf[ 5] = tt ^ iv[ 5];  iv[ 5] = tt;
				tt = ibuf[ 6];	obuf[ 6] = tt ^ iv[ 6];  iv[ 6] = tt;
				tt = ibuf[ 7];	obuf[ 7] = tt ^ iv[ 7];  iv[ 7] = tt;
				tt = ibuf[ 8];	obuf[ 8] = tt ^ iv[ 8];  iv[ 8] = tt;
				tt = ibuf[ 9];	obuf[ 9] = tt ^ iv[ 9];  iv[ 9] = tt;
				tt = ibuf[10];	obuf[10] = tt ^ iv[10];  iv[10] = tt;
				tt = ibuf[11];	obuf[11] = tt ^ iv[11];  iv[11] = tt;
				tt = ibuf[12];	obuf[12] = tt ^ iv[12];  iv[12] = tt;
				tt = ibuf[13];	obuf[13] = tt ^ iv[13];  iv[13] = tt;
				tt = ibuf[14];	obuf[14] = tt ^ iv[14];  iv[14] = tt;
				tt = ibuf[15];	obuf[15] = tt ^ iv[15];  iv[15] = tt;
#if ( nRjColumns > 4 )
				tt = ibuf[16];	obuf[16] = tt ^ iv[16];  iv[16] = tt;
				tt = ibuf[17];	obuf[17] = tt ^ iv[17];  iv[17] = tt;
				tt = ibuf[18];	obuf[18] = tt ^ iv[18];  iv[18] = tt;
				tt = ibuf[19];	obuf[19] = tt ^ iv[19];  iv[19] = tt;
				tt = ibuf[20];	obuf[20] = tt ^ iv[20];  iv[20] = tt;
				tt = ibuf[21];	obuf[21] = tt ^ iv[21];  iv[21] = tt;
				tt = ibuf[22];	obuf[22] = tt ^ iv[22];  iv[22] = tt;
				tt = ibuf[23];	obuf[23] = tt ^ iv[23];  iv[23] = tt;
#endif
#if ( nRjColumns > 6 )
				tt = ibuf[24];	obuf[24] = tt ^ iv[24];  iv[24] = tt;
				tt = ibuf[25];	obuf[25] = tt ^ iv[25];  iv[25] = tt;
				tt = ibuf[26];	obuf[26] = tt ^ iv[26];  iv[26] = tt;
				tt = ibuf[27];	obuf[27] = tt ^ iv[27];  iv[27] = tt;
				tt = ibuf[28];	obuf[28] = tt ^ iv[28];  iv[28] = tt;
				tt = ibuf[29];	obuf[29] = tt ^ iv[29];  iv[29] = tt;
				tt = ibuf[30];	obuf[30] = tt ^ iv[30];  iv[30] = tt;
				tt = ibuf[31];	obuf[31] = tt ^ iv[31];  iv[31] = tt;
#endif
			}
#if DUMP_OBUF
			fprintf( stderr, "\r\ncfb_encrypt:" );
			for ( short ii = 0; ii < AES_BLOCK_SIZE; ii++ )
				fprintf( stderr, " %02x", *(obuf+ii) );
#endif
			iBytes -= AES_BLOCK_SIZE;
			obuf += AES_BLOCK_SIZE;
			ibuf += AES_BLOCK_SIZE;
		}
		else
		{
#ifdef DUMP_OBUF
			fprintf( stderr, "\r\ncfb_encrypt:" );
#endif
			for ( unsigned short ii = 0; ii < iBytes; ii++ )
			{	*(obuf+ii) = *(ibuf+ii) ^ *(iv+ii);
#ifdef DUMP_OBUF
				fprintf( stderr, " %02x", *(obuf+ii) );
#endif
			}
			iBytes = 0;
		}
#ifdef DUMP_OBUF
		fprintf( stderr, "\r\n" );
#endif
	}	// while ( oBytes < iBytes )
}			// rjDecrypt()
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
unsigned int RotateLeft( unsigned int arg, unsigned int nBits )
{
	nBits &= 0x1F;											// in an ASM level rotate, the microarchitecture probably knows enough to do this
	unsigned int saveLeft = arg >> (8*sizeof(arg) - nBits);	// save the most significant nBits
	arg <<= nBits;											// shift the argument left to make room for the repositioned left end
	return	arg | saveLeft;									// paste the left end bits into the right end of return value
}			// RotateLeft()
//-----------------------------------------------------------------------------------------------------
// in Rijndael, KeyExpansion( unsigned char* cipherKey, unsigned int* keySchedule )
void	rcKeySpan( unsigned long* ck, unsigned long* rk )
{		//	Input:   array L[ 0 … c-1 ] of cipher key words, c :== nRcBitsInKey / nBitsPerWord = 8;
		//	Output:  array S[ 0 … 43 ]  of round key words
	unsigned int LL[ nRcWordsInKey ];							// initially, a local copy of the cipherKey
	unsigned int ll, rr;
	for ( ll = 0; ll < nRcWordsInKey; ll++ )
		*(LL+ll) = *(ck+ll);									// copy the cipherKey

		// initialize the roundKeys using the Pw and Qw constants
	const unsigned int Pw = 0xB7E15163;		// Odd( (e-2) * 2^w ), i.e. the first odd above the fractional part of e as a 32-bit integer
	const unsigned int Qw = 0x9E3779B9;		// Odd( phi-1) * 2^w ), where the golden ratio, phi = (1 + sqrt(5)) / 2
	*(rk+0) = Pw;						// seed the roundKeys with the fractional part of e
	for ( rr = 1; rr < nRcRoundKeys; rr++ )
		*(rk+rr) = *(rk+rr-1) + Qw;			// add the fractional part of phi to create successive roundKeys

		// modify roundKeys using the local copy of the cipherKey
	unsigned int AA, BB;
	AA = BB = ll = rr = 0;									// ll indexes LL (local copy of cipherKey); rr indexes roundKeys
	const unsigned int loopLimit = 3 * nRcRoundKeys;
	for ( unsigned int ss = 0; ss < loopLimit; ss++ )
	{	unsigned int AApBB = AA + BB;
		AA = *(rk+rr++) = RotateLeft( *(rk+rr) + AApBB, 3 );
		BB = *(LL+ll++) = RotateLeft( *(LL+ll) + AApBB, AApBB );
		if ( rr >= nRcRoundKeys )
			rr = 0;						// rr = rr mod nRoundKeys
		if ( ll >= nRcWordsInKey )
			ll = 0;						// ll = ii mod nWordsInKey
	}
}			// rcKeySpan()
//-----------------------------------------------------------------------------------------------------
// the Rijndael Cipher accepts unsigned char pointers, we're using unsigned ints for RC6
// void	Cipher( unsigned char* ibuf, unsigned char* obuf, const unsigned int* rk )
void	rcCipher( unsigned long* ibuf, unsigned long* obuf, const unsigned long* rk )
{		// implement RC6-32/20/256 (w/r/b)
		// there are 2(r+2) round keys.  With r=20, we have 44 round keys
		// a block consists of four 32 bit words, A, B, C, D
	register unsigned int AA = *(ibuf+0);
	register unsigned int BB = *(ibuf+1);
	register unsigned int CC = *(ibuf+2);
	register unsigned int DD = *(ibuf+3);

		// pre-whitening phase - a half round of RC5 on B and D
	BB += *(rk+0);
	DD += *(rk+1);
		// now all of the rounds on all four words
	unsigned short	jj = 0;
	for ( unsigned short ii = 0; ii < nRcRounds; ii++ )
	{	
		unsigned int tt = RotateLeft( BB * (2*BB+1), LGw );
		unsigned int uu = RotateLeft( DD * (2*DD+1), LGw );
		AA = RotateLeft( AA^tt, uu ) + *(rk + jj++);		// jj moves twice as fast as ii
		CC = RotateLeft( CC^uu, tt ) + *(rk + jj++);		// so each loop increments jj twice
			// swap the registers around
		unsigned int tmp = AA;		AA = BB;	BB = CC;	CC = DD;	DD = tmp;
	}
		// post-whitening phase - a half round of RC5 on A and C
	AA += *(rk+nRcRoundKeys-2);
	CC += *(rk+nRcRoundKeys-1);

		// oddly, rc6Cipher() was working without these four lines (???)
	*(obuf+0) = AA;
	*(obuf+1) = BB;
	*(obuf+2) = CC;
	*(obuf+3) = DD;
}			// rcCipher()
//-----------------------------------------------------------------------------------------------------
void rcCrypt(
	unsigned char *			iv,
	unsigned char *			ibuf,
	unsigned long			iBytes,
	unsigned char *			obuf,				// allocated in the caller
	const unsigned long	*	roundKeys )
{		// Refer to: http://en.wikipedia.org/wiki/Block_cipher_modes_of_operation

	while ( iBytes > 0 )
	{	rcCipher( (unsigned long*)iv, (unsigned long*)iv, roundKeys );
		if ( iBytes >= RC6_BLOCK_SIZE )
		{	if ( aligned_04(ibuf)  &&  aligned_04(obuf)  &&  aligned_04(iv) )
			{		// we always take this branch...
					// the following four statements are the unrolled version ...
				lp32(obuf)[0] = lp32(iv)[0] ^= lp32(ibuf)[0];
				lp32(obuf)[1] = lp32(iv)[1] ^= lp32(ibuf)[1];
				lp32(obuf)[2] = lp32(iv)[2] ^= lp32(ibuf)[2];
				lp32(obuf)[3] = lp32(iv)[3] ^= lp32(ibuf)[3];
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
			}
			iBytes -= RC6_BLOCK_SIZE;
			obuf += RC6_BLOCK_SIZE;
			ibuf += RC6_BLOCK_SIZE;
		}
		else 
		{	for ( unsigned short ii = 0; ii < iBytes; ii++ )
				obuf[ii] = iv[ii] ^= ibuf[ii];
			iBytes = 0;
		}
	}
}			// rcCrypt()
//-----------------------------------------------------------------------------------------------------
short	EEcode(
	char*			ibuf,
	unsigned long*	ckiv,
	char*&			mimebuf,
	DWORD&			mimelen	)
{		// encrypt and mime encode ibuf into mimebuf (a heap allocated string)
		// set mimelen to reflect the size of the mimebuf
	unsigned long	dontCare;
	int				slen_mod3;
	short			retVal = 0;		// okay return
	int	slen = strlen( ibuf );
	unsigned char* obuf = new unsigned char[slen+1];
	if ( obuf == NULL )
	{	retVal = -1;
		goto	Exit;
	}

	unsigned long rcrk[ nRcRoundKeys ];						// 44 round keys in RC6
	rcKeySpan( ckiv, (unsigned long*)rcrk );				// generate RC6 specific RoundKeys
	rcCrypt( (unsigned char*)ckiv + nRcBytesInKey, (unsigned char*)ibuf, slen, obuf, (unsigned long*)rcrk );
	*(obuf+slen) = '\0';									// righteous except URLs can't include binary data

	slen_mod3 = slen % 3;
	mimelen = 4 * ((slen+2) / 3);
	if ( slen_mod3 != 0 )
		mimelen += 3 - slen_mod3;

	if ( mimebuf )
	{	delete [] mimebuf;
		mimebuf = NULL;
	}
	mimebuf = new char[mimelen+1];
	if ( mimebuf == NULL )
	{	retVal = -2;
		goto	Exit;
	}
	dontCare = MimeEncode( obuf, slen, (unsigned char*)mimebuf );	// returns the mime length
Exit:
	if ( obuf ) delete [] obuf;
	return	retVal;
}			// EEcode()
//----------------------------------------------------------------------------------------
#endif			// ServerAccess
/*		a Rijndael256 CFB stream encryptor:
void	cfb_encrypt(
	unsigned char *			iv,
	unsigned char *			ibuf,
	unsigned long			iBytes,
	unsigned char *			obuf,				// allocated in the caller
	const unsigned int	*	roundKeys )
{		// Refer to: http://en.wikipedia.org/wiki/Block_cipher_modes_of_operation
	while ( iBytes > 0 )
	{	Cipher( iv, iv, roundKeys );
		if ( iBytes >= AES_BLOCK_SIZE )
		{	if ( aligned_04(ibuf)  &&  aligned_04(obuf)  &&  aligned_04(iv) )
			{		// we always take this branch...
					// the following four statements are the unrolled version ...
				lp32(obuf)[0] = lp32(iv)[0] ^= lp32(ibuf)[0];
				lp32(obuf)[1] = lp32(iv)[1] ^= lp32(ibuf)[1];
				lp32(obuf)[2] = lp32(iv)[2] ^= lp32(ibuf)[2];
				lp32(obuf)[3] = lp32(iv)[3] ^= lp32(ibuf)[3];
#if ( nRjColumns > 4 )
				lp32(obuf)[4] = lp32(iv)[4] ^= lp32(ibuf)[4];
				lp32(obuf)[5] = lp32(iv)[5] ^= lp32(ibuf)[5];
#endif
#if ( nRjColumns > 6 )
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
#if ( nRjColumns > 4 )
				obuf[16] = iv[16] ^= ibuf[16];	obuf[17] = iv[17] ^= ibuf[17];
				obuf[18] = iv[18] ^= ibuf[18];	obuf[19] = iv[19] ^= ibuf[19];
				obuf[20] = iv[20] ^= ibuf[20];	obuf[21] = iv[21] ^= ibuf[21];
				obuf[22] = iv[22] ^= ibuf[22];	obuf[23] = iv[23] ^= ibuf[23];
#endif
#if ( nRjColumns > 6 )
				obuf[24] = iv[24] ^= ibuf[24];	obuf[25] = iv[25] ^= ibuf[25];
				obuf[26] = iv[26] ^= ibuf[26];	obuf[27] = iv[27] ^= ibuf[27];
				obuf[28] = iv[28] ^= ibuf[28];	obuf[29] = iv[29] ^= ibuf[29];
				obuf[30] = iv[30] ^= ibuf[30];	obuf[31] = iv[31] ^= ibuf[31];
#endif
			}
			iBytes -= AES_BLOCK_SIZE;
			obuf += AES_BLOCK_SIZE;
			ibuf += AES_BLOCK_SIZE;
		}
		else 
		{	for ( short ii = 0; ii < iBytes; ii++ )
				obuf[ii] = iv[ii] ^= ibuf[ii];
			iBytes = 0;
		}
	}
}			// cfb_encrypt()
*/
//-----------------------------------------------------------------------
