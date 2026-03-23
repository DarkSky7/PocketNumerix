#include "StdAfx.h"
#include "MimeEncodeDecode.h"
#include <assert.h>

unsigned char NumToBase64( unsigned char ain )
{		// map 0 .. 63 onto { 'A' .. 'Z', 'a' .. 'z', '+', '/' }
#if _DEBUG
	assert( (ain & 0xC0) == 0 );			// the equivalent of: ain & ~0x3F == 0
#endif
	if ( ain <  26 )	return	'A' + ain;			// 0 --> 'A' .. 25 -> 'Z'
	if ( ain <  52 )	return	'a' + ain - 26;		// 26 -> 'a' .. 51 -> 'z'
	if ( ain <  62 )	return	'0' + ain - 52;		// 52 -> '0' .. 61 -> '9'
	if ( ain == 62 )	return	'+';
						return	'/';
}			// NumToBase64()
//--------------------------------------------------------------------------------------------
void	ThreeBytesIntoMIMEcodes( unsigned char ip[3], unsigned char op[4] )
{		// incoming bytes are fully packed --> outgoing bytes contain 00's in the most significant two bits
	*(op+0) = NumToBase64(								(*(ip+0) >> 2) & 0x3F  );	//					   <00><   top 6 bits of in[0]>
	*(op+1) = NumToBase64( ((*(ip+0) << 4) & 0x30)  |  ((*(ip+1) >> 4) & 0x0F) );	// <bottom 2 bits of in[0]><   top 4 bits of in[1]>
	*(op+2) = NumToBase64( ((*(ip+1) << 2) & 0x3C)  |  ((*(ip+2) >> 6) & 0x03) );	// <bottom 4 bits of in[1]><   top 2 bits of in[2]>
	*(op+3) = NumToBase64(								 *(ip+2)       & 0x3F  );	//					   <00><bottom 6 bits of in[3]>
}			// ThreeBytesIntoMIMEcodes()
//--------------------------------------------------------------------------------------------
int	MimeEncode( unsigned char* ibuf, unsigned int bytesLeft, unsigned char* obuf )
{	unsigned char* ip = ibuf;
	unsigned char* op = obuf;
	while ( bytesLeft > 0 )
	{		// three bytes into 4 MIME codes
		if ( bytesLeft >= 3 )
		{	ThreeBytesIntoMIMEcodes( ip, op );
			ip += 3;
			bytesLeft -= 3;
		}
		else	// we came up short of three binary bytes, implying end of the buffer or EOF.
		{		// since buffer size is a multiple of 3, we conclude that it's EOF
			*(op+3) = '=';										// definitely have one pad character
			*(op+0) = NumToBase64(	(*(ip+0) >> 2) & 0x3F );
			unsigned char	tmp =	(*(ip+0) << 4) & 0x30;		// needed in both cases
			if ( bytesLeft == 1 )
			{		// 1 left, out[3] = {	f(00xxxxxx), f(00xx0000), =, =				}
				*(op+1) = NumToBase64( tmp );
				*(op+2) = '=';									// with just one byte left, we need two pad characters
			}
			else	// bytesLeft == 2
			{		// 2 left, out[3] = {	f(00xxxxxx), f(00xxyyyy>, f(00yyyy00), =	}
				*(op+1) = NumToBase64( tmp  |  ((*(ip+1) >> 4) & 0x0F) );
				*(op+2) = NumToBase64(			(*(ip+1) << 2) & 0x3C  );
			}
			bytesLeft = 0;
		}
		op += 4;
	}
	*op = '\0';
	return	(int)(op - obuf);
}			// MimeEncode()
//--------------------------------------------------------------------------------------------
unsigned char Base64ToNum( unsigned char ain )
{		// map Base64 codes to numbers on the range 0x00 .. 0x3F
	if ( 'A' <= ain  &&  ain <= 'Z' )	return	ain - 'A';
	if ( 'a' <= ain  &&  ain <= 'z' )	return	ain - 'a' + 26;
	if ( '0' <= ain  &&  ain <= '9' )	return	ain - '0' + 52;
	if ( ain == '+' )					return	62;
										return	63;				// a '/'
}			// Base64ToNum()
//--------------------------------------------------------------------------------------------
void	PackSixBitCodesInThreeBytes( unsigned char ip[4], unsigned char op[3], unsigned short nValidCodes )
{		// ignore the top two bits of incoming bytes --> outgoing bytes are full of data
	if ( nValidCodes == 0 )
		return;
		// nValidCodes prevents buffer overrun on the input
    *(op+0) =	     *(ip+0) << 2;				// 00's in the least significant bits
	if ( nValidCodes > 1 )						// two, three, or four
	{	*(op+0) |= (*(ip+1) >> 4) & 0x03;		// <6 bits in[0]><2 bits in[1]>
		*(op+1)  =  *(ip+1) << 4;				// 0000's in the least significant bits
	}
	if ( nValidCodes > 2 )						// three or four
	{	*(op+1) |= (*(ip+2) >> 2) & 0x0F;		// <4 bits in[1]><4 bits in[2]>
		*(op+2)  =  *(ip+2) << 6;				// 000000 in the least significant bits
	}
	if ( nValidCodes > 3 )						// four
		*(op+2) |=  *(ip+3)	   & 0x3F;			// <2 bits in[2]><6 bits in[3]>
}			// PackSixBitCodesInThreeBytes()
//--------------------------------------------------------------------------------------------
int	MimeDecode( unsigned char* ibuf, unsigned int bytesLeft, unsigned char* obuf )
{	unsigned char* ip = ibuf;
	unsigned char* op = obuf;
	while ( bytesLeft > 0 )
	{		// 4 MIME codes into 3 binary bytes
		short	nPadChars = 0;
		if ( bytesLeft > 4 )
		{		// decode four bytes of input in place
			for ( short ii = 0; ii < 4; ii++ )
				*(ip+ii) = Base64ToNum( *(ip+ii) );
				// pack the decoded six bit binary segments into three binary bytes
			PackSixBitCodesInThreeBytes( ip, op, 4 );
		}
		else	// we found four MIME codes (hopefully, no less).  The possibilities are:
		{		// f(00xxxxxx), f(00xxyyyy), f(00yyyyzz), f(00zzzzzz)	- 4 valid codes, write three bytes
				// f(00xxxxxx), f(00xxyyyy), f(00yyyy00),	'='			- 3 valid codes, write two bytes
				// f(00xxxxxx), f(00xx0000),	 '=',		'='			- 2 valid codes, write one bytes
			if ( *(ip+3) == '=' ) nPadChars++;
			if ( *(ip+2) == '=' ) nPadChars++;
			unsigned short nValidCodes = 4 - nPadChars;					// thus 4, 3, or 2
				// decode <nValidCodes> of input in place, zero out the rest
			for ( short ii = 0; ii < nValidCodes; ii++ )
				*(ip+ii) = Base64ToNum( *(ip+ii) );						// decode to binary in place
			PackSixBitCodesInThreeBytes( ip, op, nValidCodes );
		}
		ip += 4;
		op += 3 - nPadChars;
		bytesLeft -= 4;
	}
//	*op = '\0';
	return	(int)(op - obuf);
}			// MimeDecode()
//---------------------------------------------------------------------------------
