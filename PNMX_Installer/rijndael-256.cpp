#include "StdAfx.h"
/* Rijndael Cipher

   Written by Mike Scott 21st April 1999
   Copyright (c) 1999 Mike Scott
   See rijndael documentation

   Permission for free direct or derivative use is granted subject 
   to compliance with any conditions that the originators of the 
   algorithm place on its exploitation.  

   Inspiration from Brian Gladman's implementation is acknowledged.

   Written for clarity, rather than speed.
   Full implementation. 
   Endian indifferent.
*/

// modified in order to use the libmcrypt API by Nikos Mavroyanopoulos 
// All modifications are placed under the license of libmcrypt.

/* $Id: rijndael-256.c,v 1.15 2003/01/19 17:48:27 nmav Exp $ */
//#include <libdefs.h>

//#include <mcrypt_modules.h>
#include "rijndael.h"

#define _mcrypt_set_key rijndael_256_LTX__mcrypt_set_key
#define _mcrypt_encrypt rijndael_256_LTX__mcrypt_encrypt
#define _mcrypt_decrypt rijndael_256_LTX__mcrypt_decrypt
#define _mcrypt_get_size rijndael_256_LTX__mcrypt_get_size
#define _mcrypt_get_block_size rijndael_256_LTX__mcrypt_get_block_size
#define _is_block_algorithm rijndael_256_LTX__is_block_algorithm
#define _mcrypt_get_key_size rijndael_256_LTX__mcrypt_get_key_size
#define _mcrypt_get_supported_key_sizes rijndael_256_LTX__mcrypt_get_supported_key_sizes
#define _mcrypt_get_algorithms_name rijndael_256_LTX__mcrypt_get_algorithms_name
#define _mcrypt_self_test rijndael_256_LTX__mcrypt_self_test
#define _mcrypt_algorithm_version rijndael_256_LTX__mcrypt_algorithm_version

// rotates x one bit to the left

#define ROTL(x) ( ((x)>>7)|((x)<<1) )

// Rotates 32-bit word left by 1, 2 or 3 byte
#define ROTL8(x) ( ((x)<<8)|((x)>>24) )
#define ROTL16(x) ( ((x)<<16)|((x)>>16) )
#define ROTL24(x) ( ((x)<<24)|((x)>>8) )

// Fixed Data

static byte InCo[4] = { 0xB, 0xD, 0x9, 0xE };	/* Inverse Coefficients */

static byte fbsub[256];
static byte rbsub[256];
static byte ptab[256], ltab[256];
static word32 ftable[256];
static word32 rtable[256];
static word32 rco[30];
static int tables_ok = 0;

// Parameter-dependent data in "rijndael.h" 

static word32 pack( byte* bb )
{		// pack bytes into a 32-bit Word
	return	((word32) bb[3] << 24)
		  | ((word32) bb[2] << 16)
		  | ((word32) bb[1] << 8)
		  |  (word32) bb[0];
}

static void unpack(word32 aa, byte* bb)
{		// unpack bytes from a word
	bb[0] = (byte)aa;
	bb[1] = (byte)(aa >> 8);
	bb[2] = (byte)(aa >> 16);
	bb[3] = (byte)(aa >> 24);
}

static byte xtime( byte aa )
{	byte bb;
	if ( aa & 0x80 )
		 bb = 0x1B;
	else
		bb = 0;
	aa <<= 1;
	aa ^= bb;
	return	aa;
}

static byte bmul( byte x, byte y )
{		// x.y= AntiLog(Log(x) + Log(y))
	if (x && y)
		return ptab[(ltab[x] + ltab[y]) % 255];
	else
		return 0;
}

static word32 SubByte( word32 a )
{	byte b[4];
	unpack(a, b);
	b[0] = fbsub[b[0]];
	b[1] = fbsub[b[1]];
	b[2] = fbsub[b[2]];
	b[3] = fbsub[b[3]];
	return pack(b);
}

static byte product( word32 x, word32 y )
{				/* dot product of two 4-byte arrays */
	byte xb[4], yb[4];
	unpack(x, xb);
	unpack(y, yb);
	return bmul(xb[0], yb[0]) ^ bmul(xb[1], yb[1]) ^ bmul(xb[2],
							      yb[2]) ^
	    bmul(xb[3], yb[3]);
}

static word32 InvMixCol( word32 x )
{				/* matrix Multiplication */
	word32 y, m;
	byte b[4];

	m = pack(InCo);
	b[3] = product(m, x);
	m = ROTL24(m);
	b[2] = product(m, x);
	m = ROTL24(m);
	b[1] = product(m, x);
	m = ROTL24(m);
	b[0] = product(m, x);
	y = pack(b);
	return y;
}

static byte ByteSub( byte x )
{
	byte y = ptab[255 - ltab[x]];	/* multiplicative inverse */
	x = y;
	x = ROTL(x);
	y ^= x;
	x = ROTL(x);
	y ^= x;
	x = ROTL(x);
	y ^= x;
	x = ROTL(x);
	y ^= x;
	y ^= 0x63;
	return	y;
}

static void _mcrypt_rijndael_gentables( void )
{		// generate tables
	int	ii;
	byte yy, bb[4];

		// use 3 as primitive root to generate power and log tables
	ltab[0] = 0;
	ptab[0] = 1;
	ltab[1] = 0;
	ptab[1] = 3;
	ltab[3] = 1;
	for ( ii = 2; ii < 256; ii++ )
	{
		ptab[ii] = ptab[ii-1] ^ xtime(ptab[ii - 1]);
		ltab[ptab[ii]] = ii;
	}

		// affine transformation:- each bit is xored with itself shifted one bit
	fbsub[0] = 0x63;
	rbsub[0x63] = 0;
	for ( ii = 1; ii < 256; ii++ )
	{
		yy = ByteSub((byte) ii);
		fbsub[ii] = yy;
		rbsub[yy] = ii;
	}

	for ( ii = 0, yy = 1; ii < 30; ii++ )
	{	rco[ii] = yy;
		yy = xtime(yy);
	}

		// calculate forward and reverse tables
	for ( ii = 0; ii < 256; ii++ )
	{	yy = fbsub[ii];
		bb[3] = yy ^ xtime(yy);
		bb[2] = yy;
		bb[1] = yy;
		bb[0] = xtime(yy);
		ftable[ii] = pack(bb);

		yy = rbsub[ii];
		bb[3] = bmul( InCo[0], yy );
		bb[2] = bmul( InCo[1], yy );
		bb[1] = bmul( InCo[2], yy );
		bb[0] = bmul( InCo[3], yy );
		rtable[ii] = pack(bb);
	}
}

int _mcrypt_set_key( RI* rinst, byte * key, int nk )
{		// blocksize=32*nb bits. Key=32*nk bits
		// currently nb,bk = 4, 6 or 8
		// key comes as 4*rinst->Nk bytes
		// Key Scheduler. Create expanded encryption key 
	int nb = 8;		/* 256 block size */
	int ii, jj, kk, mm, NN;
	int C1, C2, C3;
	word32 CipherKey[8];

	nk /= 4;

	if ( tables_ok == 0 )
	{	_mcrypt_rijndael_gentables();
		tables_ok = 1;
	}
	rinst->Nb = nb;
	rinst->Nk = nk;

	/* rinst->Nr is number of rounds */
	if ( rinst->Nb >= rinst->Nk )
		rinst->Nr = 6 + rinst->Nb;
	else
		rinst->Nr = 6 + rinst->Nk;

	C1 = 1;
	if ( rinst->Nb < 8 )
	{	C2 = 2;
		C3 = 3;
	}
	else
	{	C2 = 3;
		C3 = 4;
	}

	/* pre-calculate forward and reverse increments */
	for ( mm = jj = 0; jj < nb; jj++, mm += 3 )
	{	rinst->fi[mm] = (jj + C1) % nb;
		rinst->fi[mm + 1] = (jj + C2) % nb;
		rinst->fi[mm + 2] = (jj + C3) % nb;
		rinst->ri[mm] = (nb + jj - C1) % nb;
		rinst->ri[mm + 1] = (nb + jj - C2) % nb;
		rinst->ri[mm + 2] = (nb + jj - C3) % nb;
	}

	NN = rinst->Nb * (rinst->Nr + 1);

	for ( ii = jj = 0; ii < rinst->Nk; ii++, jj += 4 )
	{
		CipherKey[ii] = pack(&key[jj]);
	}
	for ( ii = 0; ii < rinst->Nk; ii++ )
		rinst->fkey[ii] = CipherKey[ii];
	for ( jj = rinst->Nk, kk = 0; jj < NN; jj += rinst->Nk, kk++)
	{
		rinst->fkey[jj] =
		    rinst->fkey[jj - rinst->Nk] ^ SubByte(ROTL24(rinst->fkey[jj - 1])) ^ rco[kk];
		if ( rinst->Nk <= 6 )
		{
			for ( ii = 1; ii < rinst->Nk && (ii + jj) < NN; ii++ )
				rinst->fkey[ii + jj] = rinst->fkey[ii + jj - rinst->Nk] ^ rinst->fkey[ii + jj - 1];
		}
		else
		{	for ( ii = 1; ii < 4 && (ii + jj) < NN; ii++ )
				rinst->fkey[ii + jj] = rinst->fkey[ii + jj - rinst->Nk] ^ rinst->fkey[ii + jj - 1];
			if ( (jj + 4) < NN )
				rinst->fkey[jj + 4] = rinst->fkey[jj + 4 - rinst->Nk] ^ SubByte(rinst->fkey[jj + 3]);
			for ( ii = 5; ii < rinst->Nk && (ii + jj) < NN; ii++)
				rinst->fkey[ii + jj] = rinst->fkey[ii + jj - rinst->Nk] ^ rinst->fkey[ii + jj - 1];
		}
	}

	/* now for the expanded decrypt key in reverse order */

	for ( jj = 0; jj < rinst->Nb; jj++ )
		rinst->rkey[jj + NN - rinst->Nb] = rinst->fkey[jj];
	for ( ii = rinst->Nb; ii < NN - rinst->Nb; ii += rinst->Nb )
	{
		kk = NN - rinst->Nb - ii;
		for ( jj = 0; jj < rinst->Nb; jj++ )
			rinst->rkey[kk + jj] = InvMixCol( rinst->fkey[ii + jj] );
	}
	for ( jj = NN - rinst->Nb; jj < NN; jj++ )
		rinst->rkey[jj - NN + rinst->Nb] = rinst->fkey[jj];
	return	0;
}

/* There is an obvious time/space trade-off possible here.     *
 * Instead of just one ftable[], I could have 4, the other     *
 * 3 pre-rotated to save the ROTL8, ROTL16 and ROTL24 overhead */

void _mcrypt_encrypt( RI* rinst, byte* buff )
{	int	ii, jj, kk, mm;
	word32 aa[8], bb[8], *xx, *yy, *tt;

	for ( ii = jj = 0; ii < rinst->Nb; ii++, jj += 4 )
	{	aa[ii] = pack( &buff[jj] );
		aa[ii] ^= rinst->fkey[ii];
	}
	kk = rinst->Nb;
	xx = aa;
	yy = bb;

		// State alternates between aa and bb
	for ( ii = 1; ii < rinst->Nr; ii++ )
	{		// rinst->Nr is number of rounds. May be odd.
			// if rinst->Nb is fixed - unroll this next 
			// loop and hard-code in the values of fi[]
		for ( mm = jj = 0; jj < rinst->Nb; jj++, mm += 3 )
		{		// deal with each 32-bit element of the State
				// This is the time-critical bit
			yy[jj] = rinst->fkey[kk++]
				   ^ ftable[(byte) xx[jj]]
				   ^ ROTL8( ftable[(byte)(xx[rinst->fi[mm]] >> 8)] )
				   ^ ROTL16( ftable[(byte)(xx[rinst->fi[mm + 1]] >> 16)] )
				   ^ ROTL24( ftable[xx[rinst->fi[mm + 2]] >> 24] );
		}
		tt = xx;
		xx = yy;
		yy = tt;		/* swap pointers */
	}

		// Last Round - unroll if possible
	for ( mm = jj = 0; jj < rinst->Nb; jj++, mm += 3 )
	{	yy[jj] = rinst->fkey[kk++]
			   ^ (word32) fbsub[(byte) xx[jj]]
			   ^ ROTL8( (word32)fbsub[(byte)(xx[rinst->fi[mm]] >> 8)] )
			   ^ ROTL16( (word32)fbsub[(byte)(xx[rinst->fi[mm + 1]] >> 16)] )
			   ^ ROTL24( (word32)fbsub[xx[rinst->fi[mm + 2]] >> 24] );
	}
	for ( ii = jj = 0; ii < rinst->Nb; ii++, jj += 4 )
	{
		unpack( yy[ii], &buff[jj] );
		xx[ii] = yy[ii] = 0;			// clean up stack
	}
	return;
}

void _mcrypt_decrypt( RI* rinst, byte* buff )
{
	int ii, jj, kk, mm;
	word32 aa[8], bb[8], *xx, *yy, *tt;
	for ( ii = jj = 0; ii < rinst->Nb; ii++, jj += 4 )
	{
		aa[ii] = pack( &buff[jj] );
		aa[ii] ^= rinst->rkey[ii];
	}
	kk = rinst->Nb;
	xx = aa;
	yy = bb;

		// State alternates between aa and bb
	for ( ii = 1; ii < rinst->Nr; ii++ )
	{		// rinst->Nr is number of rounds. May be odd.
			// if rinst->Nb is fixed - unroll this next loop and hard-code in the values of ri[]
		for ( mm = jj = 0; jj < rinst->Nb; jj++, mm += 3 )
		{		// This is the time-critical bit
			yy[jj] = rinst->rkey[kk++]
				   ^ rtable[(byte)xx[jj]]
				   ^ ROTL8( rtable[(byte)(xx[rinst->ri[mm]] >> 8)] )
				   ^ ROTL16( rtable[(byte)(xx[rinst->ri[mm + 1]] >> 16)] )
				   ^ ROTL24( rtable[xx[rinst->ri[mm + 2]] >> 24] );
		}
		tt = xx;
		xx = yy;
		yy = tt;		// swap pointers
	}

		// Last Round - unroll if possible
	for ( mm = jj = 0; jj < rinst->Nb; jj++, mm += 3 )
	{	yy[jj] = rinst->rkey[kk++]
			   ^ (word32)rbsub[(byte)xx[jj]]
			   ^ ROTL8( (word32)rbsub[(byte)(xx[rinst->ri[mm]] >> 8)] )
			   ^ ROTL16( (word32)rbsub[(byte)(xx[rinst->ri[mm + 1]] >> 16)] )
			   ^ ROTL24( (word32)rbsub[xx[rinst->ri[mm + 2]] >> 24] );
	}
	for ( ii = jj = 0; ii < rinst->Nb; ii++, jj += 4 )
	{	unpack( yy[ii], &buff[jj] );
		xx[ii] = yy[ii] = 0;					// clean up stack
	}
	return;
}

int _mcrypt_get_size()
{
	return	sizeof( RI );
}

int _mcrypt_get_block_size()
{
	return	32;
}

int _is_block_algorithm()
{
	return	1;
}

int _mcrypt_get_key_size()
{
	return	32;
}

static const int key_sizes[] = { 16, 24, 32 };
const int *_mcrypt_get_supported_key_sizes(int *len)
{
	*len = sizeof(key_sizes)/sizeof(int);
	return key_sizes;

}
char *_mcrypt_get_algorithms_name()
{
return "Rijndael-256";
}

#define CIPHER "45af6c269326fd935edd24733cff74fc1aa358841a6cd80b79f242d983f8ff2e"

int _mcrypt_self_test()
{
	unsigned char *keyword;
	unsigned char plaintext[ 32 ];
	unsigned char ciphertext[ 32 ];
	int blocksize = _mcrypt_get_block_size(), jj;
	RI* key;
	unsigned char cipher_tmp[ 200 ];

	keyword = (unsigned char*)calloc( 1, _mcrypt_get_key_size() );
	if ( keyword == NULL )
		return -1;

	for ( jj = 0; jj < _mcrypt_get_key_size(); jj++ )
	{
		keyword[jj] = ((jj * 2 + 10) % 256);
	}

	for ( jj = 0; jj < blocksize; jj++ )
	{
		plaintext[jj] = jj % 256;
	}
//	key = malloc(_mcrypt_get_size());
	key = new RI;
	if ( key == NULL )
	{
		free( keyword );
		return	-1;
	}
	memcpy( ciphertext, plaintext, blocksize );

	_mcrypt_set_key( key, keyword, _mcrypt_get_key_size());
	free( keyword );

	_mcrypt_encrypt( key, ciphertext );

	for ( jj = 0; jj < blocksize; jj++)
	{
		sprintf( &((char*)cipher_tmp)[2 * jj], "%.2x",	ciphertext[jj] );
	}

	if ( strcmp( (char*)cipher_tmp, CIPHER) != 0 )
	{
		printf( "failed compatibility\n" );
		printf( "Expected: %s\nGot: %s\n", CIPHER, (char*)cipher_tmp );
		free( key );
		return	-1;
	}
	_mcrypt_decrypt( key, ciphertext );
	free( key );

	if ( strcmp((const char*)ciphertext, (const char*)plaintext) != 0 )
	{
		printf( "failed internally\n" );
		return	-1;
	}
	return	0;
}

word32 _mcrypt_algorithm_version()
{
	return 20010801;
}

#ifdef WIN32
# ifdef USE_LTDL
int main (void)
{
       /* empty main function to avoid linker error (see cygwin FAQ) */
}
# endif
#endif
