// BitArray.cpp: implementation of the CBitArray class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "BondManagerApp.h"
#include "BitArray.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBitArray::CBitArray( unsigned short nbts ) : numBits(nbts)
{		// this can't be debugged!
	int		numInts = (numBits + 8*sizeof(int) -1) / (8*sizeof(int));
	theBits = new unsigned int[ numInts ];
		// the following TRACE never gets called!
#ifdef _DEBUG
	TRACE( _T("BitArray::CBitArray: nbts=%d, numBits=%d, numInts=%d\n"), nbts, numBits, numInts );
#endif
}

CBitArray::~CBitArray()
{
	delete	[] theBits;
}

void	CBitArray::clear( void )
{	int	numInts = (numBits + 8*sizeof(int) - 1) / (8*sizeof(int));
	for ( int ii = 0; ii < numInts; ii++ )
		*(theBits+ii) = 0;
}

void	CBitArray::set( unsigned short bitIdx )
{
	ASSERT( bitIdx < numBits );
	unsigned short		wordNo = bitIdx / (8*sizeof(int));
	unsigned short		bitNo = bitIdx - wordNo * 8 * sizeof(int);
	*(theBits+wordNo)  |=  (0x1 << bitNo);
}

void	CBitArray::reset( unsigned short bitIdx )
{
	ASSERT( bitIdx < numBits );
	unsigned short		wordNo = bitIdx / (8*sizeof(int));
	unsigned short		bitNo = bitIdx - wordNo * 8 * sizeof(int);
	*(theBits+wordNo)  &=  ~(0x1 << bitNo);
}

bool	CBitArray::query( unsigned short bitIdx )
{
	ASSERT( bitIdx < numBits );
	unsigned short		wordNo = bitIdx / (8*sizeof(int));
	unsigned short		bitNo = bitIdx - wordNo * 8 * sizeof(int);
	return (*(theBits+wordNo)  &  (0x1 << bitNo)) ? true : false;
}
