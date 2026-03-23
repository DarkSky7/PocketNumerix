#include "StdAfx.h"
#include "MimeEncode.h"
//#include <assert.h>

unsigned char NumToBase64( unsigned char ain )
{		// map 0 .. 63 onto { 'A' .. 'Z', 'a' .. 'z', '+', '/' }
#ifdef _DEBUG
	if ( (ain & 0xC0) != 0 )
		AfxDebugBreak();					// the equivalent of: ain & ~0x3F == 0
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
long	MimeEncode( unsigned char* ibuf, unsigned long bytesLeft, unsigned char* obuf )
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
			// inject whitespace every 76 MIME codes (don't think this is necessary
//		if ( ( op - obuf ) % 76 == 0 )
//			*op++ = '\n';
	}
	*op = '\0';
	return	op - obuf;
}			// MimeEncode()
//--------------------------------------------------------------------------------------------
