// Vector_f.h: interface for the CVector_f class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VECTOR_f_H__6EC38338_AB1B_438D_A3CD_3E456AD7539E__INCLUDED_)
#define AFX_VECTOR_f_H__6EC38338_AB1B_438D_A3CD_3E456AD7539E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CVector_f : public CObject
{
public:
						CVector_f( unsigned short len );
							// transform (unary apply) constructor

						CVector_f( CVector_f& av, double func( double elem) );

	virtual				~CVector_f( void );
		
		// operators
	float				operator[]( unsigned short ii );

		// methods
	void				apply( float func( float elem ) );		// for unary operations
	float				maxElem( void );
	
		// atrributes
	float*				elems;
	unsigned short		nElems;
};
#endif // !defined(AFX_VECTOR_f_H__6EC38338_AB1B_438D_A3CD_3E456AD7539E__INCLUDED_)
