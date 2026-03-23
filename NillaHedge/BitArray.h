// BitArray.h: interface for the CBitArray class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BITARRAY_H__F4EF5A94_6A61_44A3_B77E_2B2793F5F6FD__INCLUDED_)
#define AFX_BITARRAY_H__F4EF5A94_6A61_44A3_B77E_2B2793F5F6FD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CBitArray  
{
public:
	CBitArray( unsigned short numBits );
	virtual ~CBitArray();

		// operators
	bool	operator[]( unsigned short bitIdx ) { return query( bitIdx ); }

		// member functions
	void	clear( void );
	void	set( unsigned short bitIdx );
		// problems in store - numBits not getting set by instantiator!
	void	store( bool val, unsigned short bitIdx )
	{	if ( val ) set( bitIdx );
		else reset( bitIdx );
	}
	void	reset( unsigned short bitIdx );
	bool	query( unsigned short bitIdx );

		// attributes
	unsigned int*	theBits;
	unsigned short	numBits;
};

#endif // !defined(AFX_BITARRAY_H__F4EF5A94_6A61_44A3_B77E_2B2793F5F6FD__INCLUDED_)
