// Vector.h: interface for the CVector class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VECTOR_H__6EC38338_AB1B_438D_A3CD_3E456AD7539E__INCLUDED_)
#define AFX_VECTOR_H__6EC38338_AB1B_438D_A3CD_3E456AD7539E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CVector : public CObject
{
public:
						CVector( unsigned short len );
							// transform (unary apply) constructor

						CVector( CVector& av, double func( double elem) );

	virtual				~CVector( void );
		
		// operators
	double				operator[]( unsigned short ii );

		// methods
	void				apply( double func( double elem ) );		// for unary operations
	double				maxElem( void );
	
		// atrributes
	double*				elems;
	unsigned short		nElems;
};
#endif // !defined(AFX_VECTOR_H__6EC38338_AB1B_438D_A3CD_3E456AD7539E__INCLUDED_)
