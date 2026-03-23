// MimeEncodeDecode.h

int	MimeEncode( unsigned char* ibuf, unsigned int bytesLeft, unsigned char* obuf );	// returns obuf length
int	MimeDecode( unsigned char* ibuf, unsigned int bytesLeft, unsigned char* obuf );	// returns a status (0=okay, or < 0)
